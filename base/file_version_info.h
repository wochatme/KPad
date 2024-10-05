#ifndef __base_file_version_info_h__
#define __base_file_version_info_h__

#include <windows.h>
#include <string>

#include "basic_types.h"
#include "memory/scoped_ptr.h"

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

class FilePath;

class FileVersionInfo
{
public:
    ~FileVersionInfo();

    static FileVersionInfo* CreateFileVersionInfo(const FilePath& file_path);

    // Creates a FileVersionInfo for the specified module. Returns NULL in case
    // of error. The returned object should be deleted when you are done with it.
    static FileVersionInfo* CreateFileVersionInfoForModule(HMODULE module);

    __forceinline static FileVersionInfo*
        CreateFileVersionInfoForCurrentModule()
    {
        HMODULE module = reinterpret_cast<HMODULE>(&__ImageBase);
        return CreateFileVersionInfoForModule(module);
    }

    virtual std::wstring company_name();
    virtual std::wstring company_short_name();
    virtual std::wstring product_name();
    virtual std::wstring product_short_name();
    virtual std::wstring internal_name();
    virtual std::wstring product_version();
    virtual std::wstring private_build();
    virtual std::wstring special_build();
    virtual std::wstring comments();
    virtual std::wstring original_filename();
    virtual std::wstring file_description();
    virtual std::wstring file_version();
    virtual std::wstring legal_copyright();
    virtual std::wstring legal_trademarks();
    virtual std::wstring last_change();
    virtual bool is_official_build();

    bool GetValue(const wchar_t* name, std::wstring& value);

    std::wstring GetStringValue(const wchar_t* name);

    VS_FIXEDFILEINFO* fixed_file_info() { return fixed_file_info_; }

private:
    FileVersionInfo(void* data, int language, int code_page);

    scoped_ptr_malloc<char> data_;
    int language_;
    int code_page_;

    VS_FIXEDFILEINFO* fixed_file_info_;

    DISALLOW_COPY_AND_ASSIGN(FileVersionInfo);
};

#endif //__base_file_version_info_h__