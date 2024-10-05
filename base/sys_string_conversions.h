#ifndef __base_sys_string_conversions_h__
#define __base_sys_string_conversions_h__

#include <string>
#include "basic_types.h"

namespace base
{
    class StringPiece;

    std::string SysWideToUTF8(const std::wstring& wide);
    std::wstring SysUTF8ToWide(const StringPiece& utf8);

    std::string SysWideToNativeMB(const std::wstring& wide);
    std::wstring SysNativeMBToWide(const StringPiece& native_mb);

    std::wstring SysMultiByteToWide(const StringPiece& mb, uint32 code_page);
    std::string SysWideToMultiByte(const std::wstring& wide, uint32 code_page);

} //namespace base

#endif //__base_sys_string_conversions_h__