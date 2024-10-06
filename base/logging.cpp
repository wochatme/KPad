#include "logging.h"

#include <io.h>
#include <windows.h>

#define write(fd, buf, count) _write(fd, buf, static_cast<unsigned int>(count))

#define STDERR_FILENO 2

#include <algorithm>
#include <ctime>
#include <iomanip>

#include "base_switches.h"
#include "command_line.h"
#include "debug/debugger.h"
#include "debug/stack_trace.h"
#include "string_piece.h"
#include "synchronization/lock_impl.h"
#include "utf_string_conversions.h"
#include "vlog.h"

namespace base
{

    DcheckState g_dcheck_state = DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS;
    VlogInfo* g_vlog_info = NULL;

    const char* const log_severity_names[LOG_NUM_SEVERITIES] =
    {
        "INFO", "WARNING", "ERROR", "ERROR_REPORT", "FATAL"
    };

    int min_log_level = 0;

    LoggingDestination logging_destination = LOG_NONE;

    const int kAlwaysPrintErrorLevel = LOG_ERROR;

    typedef std::wstring PathString;
    PathString* log_file_name = NULL;

    HANDLE log_file = NULL;

    bool log_process_id = false;
    bool log_thread_id = false;
    bool log_timestamp = true;
    bool log_tickcount = false;

    bool show_error_dialogs = false;

    LogAssertHandlerFunction log_assert_handler = NULL;
    LogReportHandlerFunction log_report_handler = NULL;
    LogMessageHandlerFunction log_message_handler = NULL;

    int32 CurrentProcessId()
    {
        return GetCurrentProcessId();
    }

    int32 CurrentThreadId()
    {
        return GetCurrentThreadId();
    }

    uint64 TickCount()
    {
        return GetTickCount();
    }

    void CloseFile(HANDLE log)
    {
        CloseHandle(log);
    }

    void DeleteFilePath(const PathString& log_name)
    {
        DeleteFileW(log_name.c_str());
    }

    PathString GetDefaultLogFile()
    {
        wchar_t module_name[MAX_PATH];
        GetModuleFileNameW(NULL, module_name, MAX_PATH);

        PathString log_file = module_name;
        PathString::size_type last_backslash =
            log_file.rfind('\\', log_file.size());
        if (last_backslash != PathString::npos)
        {
            log_file.erase(last_backslash + 1);
        }
        log_file += L"debug.log";
        return log_file;
    }

    class LoggingLock
    {
    public:
        LoggingLock()
        {
            LockLogging();
        }

        ~LoggingLock()
        {
            UnlockLogging();
        }

        static void Init(LogLockingState lock_log, const PathChar* new_log_file)
        {
            if (initialized)
            {
                return;
            }
            lock_log_file = lock_log;
            if (lock_log_file == LOCK_LOG_FILE)
            {
                if (!log_mutex)
                {
                    std::wstring safe_name;
                    if (new_log_file)
                    {
                        safe_name = new_log_file;
                    }
                    else
                    {
                        safe_name = GetDefaultLogFile();
                    }

                    std::replace(safe_name.begin(), safe_name.end(), '\\', '/');
                    std::wstring t(L"Global\\");
                    t.append(safe_name);
                    log_mutex = ::CreateMutex(NULL, FALSE, t.c_str());

                    if (log_mutex == NULL)
                    {
#if DEBUG
                        int error = GetLastError();
                        debug::BreakDebugger();
#endif
                        return;
                    }
                }
            }
            else
            {
                log_lock = new internal::LockImpl();
            }
            initialized = true;
        }

    private:
        static void LockLogging()
        {
            if (lock_log_file == LOCK_LOG_FILE)
            {
                ::WaitForSingleObject(log_mutex, INFINITE);
            }
            else
            {
                log_lock->Lock();
            }
        }

        static void UnlockLogging()
        {
            if (lock_log_file == LOCK_LOG_FILE)
            {
                ReleaseMutex(log_mutex);
            }
            else
            {
                log_lock->Unlock();
            }
        }

        static internal::LockImpl* log_lock;

        static HANDLE log_mutex;

        static bool initialized;
        static LogLockingState lock_log_file;
    };

    // static
    bool LoggingLock::initialized = false;
    // static
    internal::LockImpl* LoggingLock::log_lock = NULL;
    // static
    LogLockingState LoggingLock::lock_log_file = LOCK_LOG_FILE;
    // static
    HANDLE LoggingLock::log_mutex = NULL;

    bool InitializeLogFileHandle()
    {
        if (log_file)
        {
            return true;
        }

        if (!log_file_name)
        {
            log_file_name = new PathString(GetDefaultLogFile());
        }

        if (logging_destination == LOG_ONLY_TO_FILE ||
            logging_destination == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG)
        {
            log_file = CreateFileW(log_file_name->c_str(), GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (log_file == INVALID_HANDLE_VALUE || log_file == NULL)
            {
                log_file = CreateFileW(L".\\debug.log", GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (log_file == INVALID_HANDLE_VALUE || log_file == NULL)
                {
                    log_file = NULL;
                    return false;
                }
            }
            SetFilePointer(log_file, 0, 0, FILE_END);
        }
        return true;
    }

    void BaseInitLoggingImpl(const PathChar* new_log_file,
        LoggingDestination logging_dest,
        LogLockingState lock_log,
        OldFileDeletionState delete_old,
        DcheckState dcheck_state)
    {
        CommandLine* command_line = CommandLine::ForCurrentProcess();
        g_dcheck_state = dcheck_state;
        delete g_vlog_info;
        g_vlog_info = NULL;

        if (command_line->HasSwitch(kV) || command_line->HasSwitch(kVModule))
        {
            g_vlog_info = new VlogInfo(command_line->GetSwitchValueASCII(kV),
                command_line->GetSwitchValueASCII(kVModule), &min_log_level);
        }

        LoggingLock::Init(lock_log, new_log_file);

        LoggingLock logging_lock;

        if (log_file)
        {
            // calling InitLogging twice or after some log call has already opened the
            // default log file will re-initialize to the new options
            CloseFile(log_file);
            log_file = NULL;
        }

        logging_destination = logging_dest;

        // ignore file options if logging is disabled or only to system
        if (logging_destination == LOG_NONE ||
            logging_destination == LOG_ONLY_TO_SYSTEM_DEBUG_LOG)
        {
            return;
        }

        if (!log_file_name)
        {
            log_file_name = new PathString();
        }
        *log_file_name = new_log_file;
        if (delete_old == DELETE_OLD_LOG_FILE)
        {
            DeleteFilePath(*log_file_name);
        }

        InitializeLogFileHandle();
    }

    void SetMinLogLevel(int level)
    {
        min_log_level = std::min(LOG_ERROR_REPORT, level);
    }

    int GetMinLogLevel()
    {
        return min_log_level;
    }

    int GetVlogVerbosity()
    {
        return std::max(-1, LOG_INFO - GetMinLogLevel());
    }

    int GetVlogLevelHelper(const char* file, size_t N)
    {
        DCHECK_GT(N, 0U);
        return g_vlog_info ? g_vlog_info->GetVlogLevel(base::StringPiece(file, N - 1))
            : GetVlogVerbosity();
    }

    void SetLogItems(bool enable_process_id, bool enable_thread_id,
        bool enable_timestamp, bool enable_tickcount)
    {
        log_process_id = enable_process_id;
        log_thread_id = enable_thread_id;
        log_timestamp = enable_timestamp;
        log_tickcount = enable_tickcount;
    }

    void SetShowErrorDialogs(bool enable_dialogs)
    {
        show_error_dialogs = enable_dialogs;
    }

    void SetLogAssertHandler(LogAssertHandlerFunction handler)
    {
        log_assert_handler = handler;
    }

    void SetLogReportHandler(LogReportHandlerFunction handler)
    {
        log_report_handler = handler;
    }

    void SetLogMessageHandler(LogMessageHandlerFunction handler)
    {
        log_message_handler = handler;
    }

    LogMessageHandlerFunction GetLogMessageHandler()
    {
        return log_message_handler;
    }

    void DisplayDebugMessageInDialog(const std::string& str)
    {
        if (str.empty())
        {
            return;
        }

        if (!show_error_dialogs)
        {
            return;
        }

        wchar_t prog_name[MAX_PATH];
        GetModuleFileNameW(NULL, prog_name, MAX_PATH);
        wchar_t* backslash = wcsrchr(prog_name, '\\');
        if (backslash)
        {
            backslash[1] = 0;
        }
        wcscat_s(prog_name, MAX_PATH, L"debug_message.exe");

        std::wstring cmdline = UTF8ToWide(str);
        if (cmdline.empty())
        {
            return;
        }

        STARTUPINFO startup_info;
        memset(&startup_info, 0, sizeof(startup_info));
        startup_info.cb = sizeof(startup_info);

        PROCESS_INFORMATION process_info;
        if (CreateProcessW(prog_name, &cmdline[0], NULL, NULL, false, 0, NULL,
            NULL, &startup_info, &process_info))
        {
            WaitForSingleObject(process_info.hProcess, INFINITE);
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        else
        {
            // debug process broken, let's just do a message box
            MessageBoxW(NULL, &cmdline[0], L"Fatal error", MB_OK | MB_ICONHAND | MB_TOPMOST);
        }
    }

    LogMessage::SaveLastError::SaveLastError() : last_error_(GetLastError()) {}

    LogMessage::SaveLastError::~SaveLastError()
    {
        SetLastError(last_error_);
    }

    LogMessage::LogMessage(const char* file, int line, LogSeverity severity, int ctr)
        : severity_(severity), file_(file), line_(line)
    {
        Init(file, line);
    }

    LogMessage::LogMessage(const char* file, int line, std::string* result)
        : severity_(LOG_FATAL), file_(file), line_(line)
    {
        Init(file, line);
        stream_ << "Check failed: " << *result;
        delete result;
    }

    LogMessage::LogMessage(const char* file, int line, LogSeverity severity,
        std::string* result) : severity_(severity), file_(file), line_(line)
    {
        Init(file, line);
        stream_ << "Check failed: " << *result;
        delete result;
    }

    LogMessage::LogMessage(const char* file, int line)
        : severity_(LOG_INFO), file_(file), line_(line)
    {
        Init(file, line);
    }

    LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
        : severity_(severity), file_(file), line_(line)
    {
        Init(file, line);
    }

    // writes the common header info to the stream
    void LogMessage::Init(const char* file, int line)
    {
        base::StringPiece filename(file);
        size_t last_slash_pos = filename.find_last_of("\\/");
        if (last_slash_pos != base::StringPiece::npos)
        {
            filename.remove_prefix(last_slash_pos + 1);
        }

        stream_ << '[';
        if (log_process_id)
        {
            stream_ << CurrentProcessId() << ':';
        }
        if (log_thread_id)
        {
            stream_ << CurrentThreadId() << ':';
        }
        if (log_timestamp)
        {
            time_t t = time(NULL);
            struct tm local_time = { 0 };
            localtime_s(&local_time, &t);
            struct tm* tm_time = &local_time;
            stream_ << std::setfill('0')
                << std::setw(2) << 1 + tm_time->tm_mon
                << std::setw(2) << tm_time->tm_mday
                << '/'
                << std::setw(2) << tm_time->tm_hour
                << std::setw(2) << tm_time->tm_min
                << std::setw(2) << tm_time->tm_sec
                << ':';
        }
        if (log_tickcount)
        {
            stream_ << TickCount() << ':';
        }
        if (severity_ >= 0)
        {
            stream_ << log_severity_names[severity_];
        }
        else
        {
            stream_ << "VERBOSE" << -severity_;
        }

        stream_ << log_severity_names[severity_] << ":" << file <<
            "(" << line << ")] ";

        message_start_ = stream_.tellp();
    }

    LogMessage::~LogMessage()
    {
#ifndef NDEBUG
        if (severity_ == LOG_FATAL)
        {
            debug::StackTrace trace;
            stream_ << std::endl;
            trace.OutputToStream(&stream_);
        }
#endif
        stream_ << std::endl;
        std::string str_newline(stream_.str());

        if (log_message_handler && log_message_handler(severity_, file_, line_,
            message_start_, str_newline))
        {
            return;
        }

        if (logging_destination == LOG_ONLY_TO_SYSTEM_DEBUG_LOG ||
            logging_destination == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG)
        {
            OutputDebugStringA(str_newline.c_str());
            fprintf(stderr, "%s", str_newline.c_str());
            fflush(stderr);
        }
        else if (severity_ >= kAlwaysPrintErrorLevel)
        {
            fprintf(stderr, "%s", str_newline.c_str());
            fflush(stderr);
        }

        LoggingLock::Init(LOCK_LOG_FILE, NULL);
        if (logging_destination != LOG_NONE &&
            logging_destination != LOG_ONLY_TO_SYSTEM_DEBUG_LOG)
        {
            LoggingLock logging_lock;

            if (InitializeLogFileHandle())
            {
                SetFilePointer(log_file, 0, 0, SEEK_END);
                DWORD num_written;
                WriteFile(log_file, static_cast<const void*>(str_newline.c_str()),
                    static_cast<DWORD>(str_newline.length()), &num_written, NULL);
            }
        }

        if (severity_ == LOG_FATAL)
        {
            if (debug::BeingDebugged())
            {
                debug::BreakDebugger();
            }
            else
            {
                if (log_assert_handler)
                {
                    // make a copy of the string for the handler out of paranoia
                    log_assert_handler(std::string(stream_.str()));
                }
                else
                {
#ifndef NDEBUG
                    DisplayDebugMessageInDialog(stream_.str());
#endif
                    // Crash the process to generate a dump.
                    debug::BreakDebugger();
                }
            }
        }
        else if (severity_ == LOG_ERROR_REPORT)
        {
            if (log_report_handler)
            {
                log_report_handler(std::string(stream_.str()));
            }
            else
            {
                DisplayDebugMessageInDialog(stream_.str());
            }
        }
    }

    SystemErrorCode GetLastSystemErrorCode()
    {
        return GetLastError();
    }

    Win32ErrorLogMessage::Win32ErrorLogMessage(const char* file, int line,
        LogSeverity severity, SystemErrorCode err, const char* module)
        : err_(err), module_(module), log_message_(file, line, severity) {}

    Win32ErrorLogMessage::Win32ErrorLogMessage(const char* file, int line,
        LogSeverity severity, SystemErrorCode err) : err_(err),
        module_(NULL), log_message_(file, line, severity) {}

    Win32ErrorLogMessage::~Win32ErrorLogMessage()
    {
        const int error_message_buffer_size = 256;
        char msgbuf[error_message_buffer_size];
        DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        HMODULE hmod;
        if (module_)
        {
            hmod = GetModuleHandleA(module_);
            if (hmod)
            {
                flags |= FORMAT_MESSAGE_FROM_HMODULE;
            }
            else
            {
                DPLOG(WARNING) << "Couldn't open module " << module_
                    << " for error message query";
            }
        }
        else
        {
            hmod = NULL;
        }
        DWORD len = FormatMessageA(flags, hmod, err_,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            msgbuf, sizeof(msgbuf) / sizeof(msgbuf[0]), NULL);
        if (len)
        {
            while ((len > 0) && isspace(static_cast<unsigned char>(msgbuf[len - 1])))
            {
                msgbuf[--len] = 0;
            }
            stream() << ": " << msgbuf;
        }
        else
        {
            stream() << ": Error " << GetLastError() << " while retrieving error "
                << err_;
        }
    }

    void CloseLogFile()
    {
        LoggingLock logging_lock;

        if (!log_file)
        {
            return;
        }

        CloseFile(log_file);
        log_file = NULL;
    }

    void RawLog(int level, const char* message)
    {
        if (level >= min_log_level)
        {
            size_t bytes_written = 0;
            const size_t message_len = strlen(message);
            int rv;
            while (bytes_written < message_len)
            {
                rv = write(STDERR_FILENO, message + bytes_written,
                    message_len - bytes_written);
                if (rv < 0)
                {
                    // Give up, nothing we can do now.
                    break;
                }
                bytes_written += rv;
            }

            if (message_len > 0 && message[message_len - 1] != '\n')
            {
                do
                {
                    rv = write(STDERR_FILENO, "\n", 1);
                    if (rv < 0)
                    {
                        // Give up, nothing we can do now.
                        break;
                    }
                } while (rv != 1);
            }
        }

        if (level == LOG_FATAL)
        {
            debug::BreakDebugger();
        }
    }

} //namespace base

std::ostream& operator<<(std::ostream& out, const wchar_t* wstr)
{
    return out << WideToUTF8(std::wstring(wstr));
}
