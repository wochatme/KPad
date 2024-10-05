#ifndef __base_native_library_h__
#define __base_native_library_h__

#include <windows.h>
#include "string16.h"

class FilePath;

namespace base
{
    typedef HMODULE NativeLibrary;

    NativeLibrary LoadNativeLibrary(const FilePath& library_path);

    NativeLibrary LoadNativeLibraryDynamically(const FilePath& library_path);

    void UnloadNativeLibrary(NativeLibrary library);

    void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
        const char* name);

    string16 GetNativeLibraryName(const string16& name);

} //namespace base

#endif //__base_native_library_h__