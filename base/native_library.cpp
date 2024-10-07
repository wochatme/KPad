#include "native_library.h"

#include "file_path.h"
#include "file_util.h"
#include "threading/thread_restrictions.h"
#include "utf_string_conversions.h"

namespace base
{

    typedef HMODULE(WINAPI* LoadLibraryFunction)(const wchar_t* file_name);

    NativeLibrary LoadNativeLibraryHelper(const FilePath& library_path,
        LoadLibraryFunction load_library_api)
    {
        ThreadRestrictions::AssertIOAllowed();

        bool restore_directory = false;
        FilePath current_directory;
        if (base::GetCurrentDirectory(&current_directory))
        {
            FilePath plugin_path = library_path.DirName();
            if (!plugin_path.empty())
            {
                base::SetCurrentDirectory(plugin_path);
                restore_directory = true;
            }
        }

        HMODULE module = (*load_library_api)(library_path.value().c_str());
        if (restore_directory)
        {
            base::SetCurrentDirectory(current_directory);
        }

        return module;
    }

    NativeLibrary LoadNativeLibrary(const FilePath& library_path)
    {
        return LoadNativeLibraryHelper(library_path, LoadLibraryW);
    }

    NativeLibrary LoadNativeLibraryDynamically(const FilePath& library_path)
    {
        LoadLibraryFunction load_library;
        load_library = reinterpret_cast<LoadLibraryFunction>(
            GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW"));

        return LoadNativeLibraryHelper(library_path, load_library);
    }

    void UnloadNativeLibrary(NativeLibrary library)
    {
        FreeLibrary(library);
    }

    void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
        const char* name)
    {
        return GetProcAddress(library, name);
    }

    string16 GetNativeLibraryName(const string16& name)
    {
        return name + ASCIIToUTF16(".dll");
    }

} //namespace base


