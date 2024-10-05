#ifndef __base_file_util_h__
#define __base_file_util_h__

#include <stack>

#include "file_path.h"
#include "platform_file.h"

namespace base
{
    // Convert provided relative path into an absolute path.  Returns false on
    // error. On POSIX, this function fails if the path does not exist.
    bool AbsolutePath(FilePath* path);

    int CountFilesCreatedAfter(const FilePath& path,
        const Time& comparison_time);

    int64 ComputeDirectorySize(const FilePath& root_path);

    int64 ComputeFilesSize(const FilePath& directory,
        const std::wstring& pattern);

    bool Delete(const FilePath& path, bool recursive);

    bool DeleteAfterReboot(const FilePath& path);

    bool GetFileInfo(const FilePath& file_path, PlatformFileInfo* info);

    FILE* OpenFile(const FilePath& filename, const char* mode);

    bool CloseFile(FILE* file);

    bool GetTempDir(FilePath* path);

    bool CreateTemporaryFile(FilePath* path);

    bool CreateTemporaryFileInDir(const FilePath& dir, FilePath* temp_file);

    FILE* CreateAndOpenTemporaryFile(FilePath* path);

    FILE* CreateAndOpenTemporaryFileInDir(const FilePath& dir, FilePath* path);

    bool ReadFileToString(const FilePath& path, std::string* contents);

    int ReadFile(const FilePath& filename, char* data, int size);

    int WriteFile(const FilePath& filename, const char* data, int size);

    // Moves the given path, whether it's a file or a directory.
    // If a simple rename is not possible, such as in the case where the paths are
    // on different volumes, this will attempt to copy and delete. Returns
    // true for success.
    bool Move(const FilePath& from_path, const FilePath& to_path);

    bool ReplaceFile(const FilePath& from_path, const FilePath& to_path);

    bool PathExists(const FilePath& path);

    // Returns true if the given path exists and is a directory, false otherwise.
    bool DirectoryExists(const FilePath& path);

    bool GetCurrentDirectory(FilePath* path);

    bool SetCurrentDirectory(const FilePath& path);

    // Copies the given path, and optionally all subdirectories and their contents
    // as well.
    // If there are files existing under to_path, always overwrite.
    // Returns true if successful, false otherwise.
    // Don't use wildcards on the names, it may stop working without notice.
    //
    // If you only need to copy a file use CopyFile, it's faster.
    bool CopyDirectory(const FilePath& from_path,
        const FilePath& to_path,
        bool recursive);

    // Copy from_path to to_path recursively and then delete from_path recursively.
    // Returns true if all operations succeed.
    // This function simulates Move(), but unlike Move() it works across volumes.
    // This fuction is not transactional.
    bool CopyAndDeleteDirectory(const FilePath& from_path, const FilePath& to_path);

    // Create a new directory. If prefix is provided, the new directory name is in
    // the format of prefixyyyy.
    // NOTE: prefix is ignored in the POSIX implementation.
    // If success, return true and output the full path of the directory created.
    bool CreateNewTempDirectory(const FilePath::StringType& prefix,
        FilePath* new_temp_path);

    // Create a directory within another directory.
    // Extra characters will be appended to |prefix| to ensure that the
    // new directory does not have the same name as an existing directory.
    bool CreateTemporaryDirInDir(const FilePath& base_dir,
        const FilePath::StringType& prefix,
        FilePath* new_dir);

    // Creates a directory, as well as creating any parent directories, if they
    // don't exist. Returns 'true' on successful creation, or if the directory
    // already exists.  The directory is only readable by the current user.
    bool CreateDirectory(const FilePath& full_path);

    // Returns true if the given path's base name is ".".
    bool IsDot(const FilePath& path);

    // Returns true if the given path's base name is "..".
    bool IsDotDot(const FilePath& path);

    class FileEnumerator
    {
    public:
        typedef WIN32_FIND_DATA FindInfo;

        enum FileType
        {
            FILES = 1 << 0,
            DIRECTORIES = 1 << 1,
            INCLUDE_DOT_DOT = 1 << 2,
        };

        FileEnumerator(const FilePath& root_path,
            bool recursive,
            FileType file_type);
        FileEnumerator(const FilePath& root_path,
            bool recursive,
            FileType file_type,
            const FilePath::StringType& pattern);
        ~FileEnumerator();

        FilePath Next();

        void GetFindInfo(FindInfo* info);

        static bool IsDirectory(const FindInfo& info);

        static FilePath GetFilename(const FindInfo& find_info);
        static int64 GetFilesize(const FindInfo& find_info);
        static base::Time GetLastModifiedTime(const FindInfo& find_info);

    private:
        bool ShouldSkip(const FilePath& path);

        bool has_find_data_;
        WIN32_FIND_DATA find_data_;
        HANDLE find_handle_;

        FilePath root_path_;
        bool recursive_;
        FileType file_type_;
        std::wstring pattern_; 

        std::stack<FilePath> pending_paths_;

        DISALLOW_COPY_AND_ASSIGN(FileEnumerator);
    };


    class MemoryMappedFile
    {
    public:
        MemoryMappedFile();
        ~MemoryMappedFile();

        bool Initialize(const FilePath& file_name);
        bool Initialize(PlatformFile file);

        const uint8* data() const { return data_; }
        size_t length() const { return length_; }

        bool IsValid();

    private:
        bool MapFileToMemory(const FilePath& file_name);

        bool MapFileToMemoryInternal();

        bool MapFileToMemoryInternalEx(int flags);

        void CloseHandles();

        PlatformFile file_;
        HANDLE file_mapping_;
        uint8* data_;
        size_t length_;

        DISALLOW_COPY_AND_ASSIGN(MemoryMappedFile);
    };

} //namespace base

#endif //__base_file_util_h__