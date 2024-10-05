#ifndef __base_command_line_h__
#define __base_command_line_h__

#include <map>
#include <string>
#include <vector>

class FilePath;

class CommandLine
{
public:
    typedef std::wstring StringType;

    typedef StringType::value_type CharType;
    typedef std::vector<StringType> StringVector;
    typedef std::map<std::string, StringType> SwitchMap;

    enum NoProgram { NO_PROGRAM };
    explicit CommandLine(NoProgram no_program);

    explicit CommandLine(const FilePath& program);

    // Construct a new command line from an argument list.
    CommandLine(int argc, const CharType* const* argv);
    explicit CommandLine(const StringVector& argv);

    ~CommandLine();

    static void Init(int argc, const char* const* argv);

    static void Reset();

    static CommandLine* ForCurrentProcess();

    static CommandLine FromString(const std::wstring& command_line);

    // Initialize from an argv vector.
    void InitFromArgv(int argc, const CharType* const* argv);
    void InitFromArgv(const StringVector& argv);

    StringType GetCommandLineString() const;

    // Returns the original command line string as a vector of strings.
    const StringVector& argv() const { return argv_; }

    FilePath GetProgram() const;
    void SetProgram(const FilePath& program);

    bool HasSwitch(const std::string& switch_string) const;

    std::string GetSwitchValueASCII(const std::string& switch_string) const;
    FilePath GetSwitchValuePath(const std::string& switch_string) const;
    StringType GetSwitchValueNative(const std::string& switch_string) const;

    const SwitchMap& GetSwitches() const { return switches_; }

    void AppendSwitch(const std::string& switch_string);
    void AppendSwitchPath(const std::string& switch_string,
        const FilePath& path);
    void AppendSwitchNative(const std::string& switch_string,
        const StringType& value);
    void AppendSwitchASCII(const std::string& switch_string,
        const std::string& value);

    void CopySwitchesFrom(const CommandLine& source,
        const char* const switches[], size_t count);

    // Get the remaining arguments to the command.
    StringVector GetArgs() const;

    void AppendArg(const std::string& value);
    void AppendArgPath(const FilePath& value);
    void AppendArgNative(const StringType& value);

    void AppendArguments(const CommandLine& other, bool include_program);

    void PrependWrapper(const StringType& wrapper);

    void ParseFromString(const std::wstring& command_line);

private:
    CommandLine();

    static CommandLine* current_process_commandline_;

    // The argv array: { program, [(--|-|/)switch[=value]]*, [--], [argument]* }
    StringVector argv_;

    SwitchMap switches_;

    // The index after the program and switches, any arguments start here.
    size_t begin_args_;
};

#endif //__base_command_line_h__