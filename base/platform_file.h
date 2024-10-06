#ifndef __base_platform_file_h__
#define __base_platform_file_h__

#include "basic_types.h"
#include "file_path.h"
#include "base_time.h"

namespace base
{

    typedef HANDLE PlatformFile;
    const PlatformFile kInvalidPlatformFileValue = INVALID_HANDLE_VALUE;

    enum PlatformFileFlags
    {
        PLATFORM_FILE_OPEN = 1,
        PLATFORM_FILE_CREATE = 2,
        PLATFORM_FILE_OPEN_ALWAYS = 4, 
        PLATFORM_FILE_CREATE_ALWAYS = 8,
        PLATFORM_FILE_READ = 16,
        PLATFORM_FILE_WRITE = 32,
        PLATFORM_FILE_EXCLUSIVE_READ = 64,
        PLATFORM_FILE_EXCLUSIVE_WRITE = 128,
        PLATFORM_FILE_ASYNC = 256,
        PLATFORM_FILE_TEMPORARY = 512,    
        PLATFORM_FILE_HIDDEN = 1024,      
        PLATFORM_FILE_DELETE_ON_CLOSE = 2048,
        PLATFORM_FILE_TRUNCATE = 4096,
        PLATFORM_FILE_WRITE_ATTRIBUTES = 8192, 

        PLATFORM_FILE_SHARE_DELETE = 32768,  
    };

    enum PlatformFileError
    {
        PLATFORM_FILE_OK = 0,
        PLATFORM_FILE_ERROR_FAILED = -1,
        PLATFORM_FILE_ERROR_IN_USE = -2,
        PLATFORM_FILE_ERROR_EXISTS = -3,
        PLATFORM_FILE_ERROR_NOT_FOUND = -4,
        PLATFORM_FILE_ERROR_ACCESS_DENIED = -5,
        PLATFORM_FILE_ERROR_TOO_MANY_OPENED = -6,
        PLATFORM_FILE_ERROR_NO_MEMORY = -7,
        PLATFORM_FILE_ERROR_NO_SPACE = -8,
        PLATFORM_FILE_ERROR_NOT_A_DIRECTORY = -9,
        PLATFORM_FILE_ERROR_INVALID_OPERATION = -10,
        PLATFORM_FILE_ERROR_SECURITY = -11,
        PLATFORM_FILE_ERROR_ABORT = -12,
        PLATFORM_FILE_ERROR_NOT_A_FILE = -13,
        PLATFORM_FILE_ERROR_NOT_EMPTY = -14,
    };

    struct PlatformFileInfo
    {
        PlatformFileInfo();
        ~PlatformFileInfo();

        int64 size;

        bool is_directory;

        bool is_symbolic_link;

        base::Time last_modified;

        base::Time last_accessed;

        base::Time creation_time;
    };

    PlatformFile CreatePlatformFile(const FilePath& name,
        int flags, bool* created, PlatformFileError* error_code);

    bool ClosePlatformFile(PlatformFile file);

    // Reads the given number of bytes (or until EOF is reached) starting with the
    // given offset. Returns the number of bytes read, or -1 on error. Note that
    // this function makes a best effort to read all data on all platforms, so it is
    // not intended for stream oriented files but instead for cases when the normal
    // expectation is that actually |size| bytes are read unless there is an error.
    int ReadPlatformFile(PlatformFile file, int64 offset, char* data, int size);

    // Reads the given number of bytes (or until EOF is reached) starting with the
    // given offset, but does not make any effort to read all data on all platforms.
    // Returns the number of bytes read, or -1 on error.
    int ReadPlatformFileNoBestEffort(PlatformFile file, int64 offset,
        char* data, int size);

    // Writes the given buffer into the file at the given offset, overwritting any
    // data that was previously there. Returns the number of bytes written, or -1
    // on error. Note that this function makes a best effort to write all data on
    // all platforms.
    int WritePlatformFile(PlatformFile file, int64 offset,
        const char* data, int size);

    bool TruncatePlatformFile(PlatformFile file, int64 length);

    bool FlushPlatformFile(PlatformFile file);

    bool TouchPlatformFile(PlatformFile file, const Time& last_access_time,
        const Time& last_modified_time);

    bool GetPlatformFileInfo(PlatformFile file, PlatformFileInfo* info);

    class PassPlatformFile
    {
    public:
        explicit PassPlatformFile(PlatformFile* value) : value_(value) {}

        PlatformFile ReleaseValue()
        {
            PlatformFile temp = *value_;
            *value_ = kInvalidPlatformFileValue;
            return temp;
        }

    private:
        PlatformFile* value_;
    };

} //namespace base

#endif //__base_platform_file_h__