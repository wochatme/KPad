#ifndef __base_logging_h__
#define __base_logging_h__

#include <string>
#include <sstream>

#include "basic_types.h"

namespace base
{
    enum LoggingDestination
    {
        LOG_NONE,
        LOG_ONLY_TO_FILE,
        LOG_ONLY_TO_SYSTEM_DEBUG_LOG,
        LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG
    };

    enum LogLockingState { LOCK_LOG_FILE, DONT_LOCK_LOG_FILE };

    enum OldFileDeletionState { DELETE_OLD_LOG_FILE, APPEND_TO_OLD_LOG_FILE };

    enum DcheckState
    {
        DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS,
        ENABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS
    };

    typedef wchar_t PathChar;

#if NDEBUG
#define BaseInitLoggingImpl BaseInitLoggingImpl_built_with_NDEBUG
#else
#define BaseInitLoggingImpl BaseInitLoggingImpl_built_without_NDEBUG
#endif

    void BaseInitLoggingImpl(const PathChar* log_file,
        LoggingDestination logging_dest,
        LogLockingState lock_log,
        OldFileDeletionState delete_old,
        DcheckState dcheck_state);

    void SetMinLogLevel(int level);

    int GetMinLogLevel();

    int GetVlogVerbosity();

    int GetVlogLevelHelper(const char* file_start, size_t N);

    template<size_t N>
    int GetVlogLevel(const char(&file)[N])
    {
        return GetVlogLevelHelper(file, N);
    }

    void SetLogItems(bool enable_process_id, bool enable_thread_id,
        bool enable_timestamp, bool enable_tickcount);

    void SetShowErrorDialogs(bool enable_dialogs);

    typedef void (*LogAssertHandlerFunction)(const std::string& str);
    void SetLogAssertHandler(LogAssertHandlerFunction handler);

    typedef void (*LogReportHandlerFunction)(const std::string& str);
    void SetLogReportHandler(LogReportHandlerFunction handler);

    typedef bool (*LogMessageHandlerFunction)(int severity, const char* file,
        int line, size_t message_start, const std::string& str);
    void SetLogMessageHandler(LogMessageHandlerFunction handler);
    LogMessageHandlerFunction GetLogMessageHandler();

    typedef int LogSeverity;
    const LogSeverity LOG_VERBOSE = -1; 
    const LogSeverity LOG_INFO = 0;
    const LogSeverity LOG_WARNING = 1;
    const LogSeverity LOG_ERROR = 2;
    const LogSeverity LOG_ERROR_REPORT = 3;
    const LogSeverity LOG_FATAL = 4;
    const LogSeverity LOG_NUM_SEVERITIES = 5;
#ifdef NDEBUG
    const LogSeverity LOG_DFATAL = LOG_ERROR;
#else
    const LogSeverity LOG_DFATAL = LOG_FATAL;
#endif

#define COMPACT_LOG_EX_INFO(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_INFO, ##__VA_ARGS__)
#define COMPACT_LOG_EX_WARNING(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_WARNING, ##__VA_ARGS__)
#define COMPACT_LOG_EX_ERROR(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_ERROR, ##__VA_ARGS__)
#define COMPACT_LOG_EX_ERROR_REPORT(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_ERROR_REPORT, ##__VA_ARGS__)
#define COMPACT_LOG_EX_FATAL(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_FATAL, ##__VA_ARGS__)
#define COMPACT_LOG_EX_DFATAL(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_DFATAL, ##__VA_ARGS__)

#define COMPACT_LOG_INFO                    COMPACT_LOG_EX_INFO(LogMessage)
#define COMPACT_LOG_WARNING                 COMPACT_LOG_EX_WARNING(LogMessage)
#define COMPACT_LOG_ERROR                   COMPACT_LOG_EX_ERROR(LogMessage)
#define COMPACT_LOG_ERROR_REPORT            COMPACT_LOG_EX_ERROR_REPORT(LogMessage)
#define COMPACT_LOG_FATAL                   COMPACT_LOG_EX_FATAL(LogMessage)
#define COMPACT_LOG_DFATAL                  COMPACT_LOG_EX_DFATAL(LogMessage)

#define ERROR 0
#define COMPACT_LOG_EX_0(ClassName, ...)    COMPACT_LOG_EX_ERROR(ClassName, ##__VA_ARGS__)
#define COMPACT_LOG_0                       COMPACT_LOG_ERROR
    const LogSeverity LOG_0 = LOG_ERROR;

#define LOG_IS_ON(severity)                 ((base::LOG_ ## severity) >= base::GetMinLogLevel())

#define VLOG_IS_ON(verboselevel)            ((verboselevel) <= base::GetVlogLevel(__FILE__))

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : base::LogMessageVoidify()&(stream)

#define LOG_STREAM(severity)                COMPACT_LOG_ ## severity.stream()
#define LOG(severity)                       LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition)         LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity)&&(condition))

#define VLOG_STREAM(verbose_level)          base::LogMessage(__FILE__, __LINE__, -verbose_level).stream()
#define VLOG(verbose_level)                 LAZY_STREAM(VLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level))
#define VLOG_IF(verbose_level, condition)   LAZY_STREAM(VLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level)&&(condition))

#define LOG_ASSERT(condition)               LOG_IF(FATAL, !(condition)) << "Assert failed: " #condition ". "

#define LOG_GETLASTERROR_STREAM(severity)   COMPACT_LOG_EX_ ## severity(Win32ErrorLogMessage, base::GetLastSystemErrorCode()).stream()
#define LOG_GETLASTERROR(severity)          LAZY_STREAM(LOG_GETLASTERROR_STREAM(severity), LOG_IS_ON(severity))
#define LOG_GETLASTERROR_MODULE_STREAM(severity, module) \
    COMPACT_LOG_EX_ ## severity(Win32ErrorLogMessage, base::GetLastSystemErrorCode(), module).stream()
#define LOG_GETLASTERROR_MODULE(severity, module) \
    LAZY_STREAM(LOG_GETLASTERROR_MODULE_STREAM(severity, module), LOG_IS_ON(severity))

#define PLOG_STREAM(severity)               LOG_GETLASTERROR_STREAM(severity)
#define PLOG(severity)                      LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))
#define PLOG_IF(severity, condition)        LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity)&&(condition))

#define CHECK(condition)                    LAZY_STREAM(LOG_STREAM(FATAL), !(condition)) << "Check failed: " #condition ". "
#define PCHECK(condition)                   LAZY_STREAM(PLOG_STREAM(FATAL), !(condition)) << "Check failed: " #condition ". "

    template<typename t1, typename t2>
    std::string* MakeCheckOpString(const t1& v1, const t2& v2, const char* names)
    {
        std::ostringstream ss;
        ss << names << " (" << v1 << " vs. " << v2 << ")";
        std::string* msg = new std::string(ss.str());
        return msg;
    }

#define CHECK_OP(name, op, val1, val2) \
    if(std::string* _result = \
        base::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
        base::LogMessage(__FILE__, __LINE__, _result).stream()

#define DEFINE_CHECK_OP_IMPL(name, op) \
    template<typename t1, typename t2> \
    inline std::string* Check##name##Impl(const t1& v1, const t2& v2, const char* names) \
    { \
        if(v1 op v2) return NULL; \
        else return MakeCheckOpString(v1, v2, names); \
    } \
    inline std::string* Check##name##Impl(int v1, int v2, const char* names) \
    { \
        if(v1 op v2) return NULL; \
        else return MakeCheckOpString(v1, v2, names); \
    }

    DEFINE_CHECK_OP_IMPL(EQ, == )
        DEFINE_CHECK_OP_IMPL(NE, != )
        DEFINE_CHECK_OP_IMPL(LE, <= )
        DEFINE_CHECK_OP_IMPL(LT, < )
        DEFINE_CHECK_OP_IMPL(GE, >= )
        DEFINE_CHECK_OP_IMPL(GT, > )
#undef DEFINE_CHECK_OP_IMPL

#define CHECK_EQ(val1, val2)    CHECK_OP(EQ, ==, val1, val2)
#define CHECK_NE(val1, val2)    CHECK_OP(NE, !=, val1, val2)
#define CHECK_LE(val1, val2)    CHECK_OP(LE, <=, val1, val2)
#define CHECK_LT(val1, val2)    CHECK_OP(LT, < , val1, val2)
#define CHECK_GE(val1, val2)    CHECK_OP(GE, >=, val1, val2)
#define CHECK_GT(val1, val2)    CHECK_OP(GT, > , val1, val2)

#if (defined(OS_WIN) && defined(OFFICIAL_BUILD))
#define LOGGING_IS_OFFICIAL_BUILD

#define ENABLE_DLOG     0
#define ENABLE_DCHECK   0
#elif defined(NDEBUG)
#define ENABLE_DLOG     0
#define ENABLE_DCHECK   1
#else
#define ENABLE_DLOG     1
#define ENABLE_DCHECK   1
#endif

#if ENABLE_DLOG
#define DLOG_IS_ON(severity)                LOG_IS_ON(severity)
#define DLOG_IF(severity, condition)        LOG_IF(severity, condition)
#define DLOG_ASSERT(condition)              LOG_ASSERT(condition)
#define DPLOG_IF(severity, condition)       PLOG_IF(severity, condition)
#define DVLOG_IF(verboselevel, condition)   VLOG_IF(verboselevel, condition)
#else //ENABLE_DLOG
#define DLOG_EAT_STREAM_PARAMETERS          true ? (void)0 : base::LogMessageVoidify()&LOG_STREAM(FATAL)

#define DLOG_IS_ON(severity)                false
#define DLOG_IF(severity, condition)        DLOG_EAT_STREAM_PARAMETERS
#define DLOG_ASSERT(condition)              DLOG_EAT_STREAM_PARAMETERS
#define DPLOG_IF(severity, condition)       DLOG_EAT_STREAM_PARAMETERS
#define DVLOG_IF(verboselevel, condition)   DLOG_EAT_STREAM_PARAMETERS
#endif //ENABLE_DLOG
        enum { DEBUG_MODE = ENABLE_DLOG };

#undef ENABLE_DLOG

#define DLOG(severity)                      LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))

#define DLOG_GETLASTERROR(severity)         LAZY_STREAM(LOG_GETLASTERROR_STREAM(severity), DLOG_IS_ON(severity))
#define DLOG_GETLASTERROR_MODULE(severity, module) \
    LAZY_STREAM(LOG_GETLASTERROR_MODULE_STREAM(severity, module), DLOG_IS_ON(severity))

#define DPLOG(severity)                     LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))
#define DVLOG(verboselevel)                 DLOG_IF(INFO, VLOG_IS_ON(verboselevel))

#if ENABLE_DCHECK

#if defined(NDEBUG)
#define COMPACT_LOG_EX_DCHECK(ClassName, ...) \
    COMPACT_LOG_EX_ERROR_REPORT(ClassName , ##__VA_ARGS__)
#define COMPACT_LOG_DCHECK COMPACT_LOG_ERROR_REPORT
    const LogSeverity LOG_DCHECK = LOG_ERROR_REPORT;
    extern DcheckState g_dcheck_state;
#define DCHECK_IS_ON() ((base::g_dcheck_state == \
    base::ENABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS) && \
        LOG_IS_ON(DCHECK))
#else //defined(NDEBUG)
#define COMPACT_LOG_EX_DCHECK(ClassName, ...) \
    COMPACT_LOG_EX_FATAL(ClassName , ##__VA_ARGS__)
#define COMPACT_LOG_DCHECK COMPACT_LOG_FATAL
    const LogSeverity LOG_DCHECK = LOG_FATAL;
#define DCHECK_IS_ON() true
#endif //defined(NDEBUG)

#else //!ENABLE_DCHECK

#define COMPACT_LOG_EX_DCHECK(ClassName, ...) \
    COMPACT_LOG_EX_INFO(ClassName , ##__VA_ARGS__)
#define COMPACT_LOG_DCHECK COMPACT_LOG_INFO
    const LogSeverity LOG_DCHECK = LOG_INFO;
#define DCHECK_IS_ON() false

#endif //ENABLE_DCHECK
#undef ENABLE_DCHECK

#define DCHECK(condition) \
    LAZY_STREAM(LOG_STREAM(DCHECK), DCHECK_IS_ON() && !(condition)) \
        << "Check failed: " #condition ". "

#define DPCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(DCHECK), DCHECK_IS_ON() && !(condition)) \
        << "Check failed: " #condition ". "

#define DCHECK_OP(name, op, val1, val2) \
    if(DCHECK_IS_ON()) \
    if(std::string* _result = \
        base::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
        base::LogMessage(__FILE__, __LINE__, base::LOG_DCHECK, _result).stream()

#define DCHECK_EQ(val1, val2)        DCHECK_OP(EQ, ==, val1, val2)
#define DCHECK_NE(val1, val2)        DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_LE(val1, val2)        DCHECK_OP(LE, <=, val1, val2)
#define DCHECK_LT(val1, val2)        DCHECK_OP(LT, < , val1, val2)
#define DCHECK_GE(val1, val2)        DCHECK_OP(GE, >=, val1, val2)
#define DCHECK_GT(val1, val2)        DCHECK_OP(GT, > , val1, val2)

#define NOTREACHED() DCHECK(false)

#undef assert
#define assert(x) DLOG_ASSERT(x)

    class LogMessage
    {
    public:
        LogMessage(const char* file, int line, LogSeverity severity, int ctr);

        LogMessage(const char* file, int line);

        LogMessage(const char* file, int line, LogSeverity severity);

        LogMessage(const char* file, int line, std::string* result);

        LogMessage(const char* file, int line, LogSeverity severity,
            std::string* result);

        ~LogMessage();

        std::ostream& stream() { return stream_; }

    private:
        void Init(const char* file, int line);

        LogSeverity severity_;
        std::ostringstream stream_;
        size_t message_start_; 

        const char* file_;
        const int line_;

        class SaveLastError
        {
        public:
            SaveLastError();
            ~SaveLastError();

            unsigned long get_error() const { return last_error_; }

        protected:
            unsigned long last_error_;
        };

        SaveLastError last_error_;

        DISALLOW_COPY_AND_ASSIGN(LogMessage);
    };

    inline void LogAtLevel(const int log_level, const std::string& msg)
    {
        LogMessage(__FILE__, __LINE__, log_level).stream() << msg;
    }

    class LogMessageVoidify
    {
    public:
        LogMessageVoidify() {}
        void operator&(std::ostream&) {}
    };

    typedef unsigned long SystemErrorCode;
    SystemErrorCode GetLastSystemErrorCode();

    class Win32ErrorLogMessage
    {
    public:
        Win32ErrorLogMessage(const char* file, int line, LogSeverity severity,
            SystemErrorCode err, const char* module);

        Win32ErrorLogMessage(const char* file, int line, LogSeverity severity,
            SystemErrorCode err);

        // Appends the error message before destructing the encapsulated class.
        ~Win32ErrorLogMessage();

        std::ostream& stream() { return log_message_.stream(); }

    private:
        SystemErrorCode err_;
        const char* module_;
        LogMessage log_message_;

        DISALLOW_COPY_AND_ASSIGN(Win32ErrorLogMessage);
    };

    void CloseLogFile();

    void RawLog(int level, const char* message);

#define RAW_LOG(level, message) base::RawLog(base::LOG_ ## level, message)

#define RAW_CHECK(condition) \
    do \
    { \
        if(!(condition)) \
            base::RawLog(base::LOG_FATAL, "Check failed: " #condition "\n"); \
    } while(0)

} //namespace base

std::ostream& operator<<(std::ostream& out, const wchar_t* wstr);
inline std::ostream& operator<<(std::ostream& out, const std::wstring& wstr)
{
    return out << wstr.c_str();
}

#ifndef NOTIMPLEMENTED_POLICY
#define NOTIMPLEMENTED_POLICY 4
#endif

#define NOTIMPLEMENTED_MSG "NOT IMPLEMENTED"

#if NOTIMPLEMENTED_POLICY == 0
#define NOTIMPLEMENTED()
#elif NOTIMPLEMENTED_POLICY == 1
// TODO, figure out how to generate a warning
#define NOTIMPLEMENTED() COMPILE_ASSERT(false, NOT_IMPLEMENTED)
#elif NOTIMPLEMENTED_POLICY == 2
#define NOTIMPLEMENTED() COMPILE_ASSERT(false, NOT_IMPLEMENTED)
#elif NOTIMPLEMENTED_POLICY == 3
#define NOTIMPLEMENTED() NOTREACHED()
#elif NOTIMPLEMENTED_POLICY == 4
#define NOTIMPLEMENTED() LOG(ERROR) << NOTIMPLEMENTED_MSG
#elif NOTIMPLEMENTED_POLICY == 5
#define NOTIMPLEMENTED()\
    do \
    { \
        static int count = 0; \
        LOG_IF(ERROR, 0==count++) << NOTIMPLEMENTED_MSG; \
    } while(0)
#endif

namespace base
{
    class StringPiece;
}
extern std::ostream& operator<<(std::ostream& o, const base::StringPiece& piece);

#endif //__base_logging_h__