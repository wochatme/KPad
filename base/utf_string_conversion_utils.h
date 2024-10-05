#ifndef __base_utf_string_conversion_utils_h__
#define __base_utf_string_conversion_utils_h__

#include "basic_types.h"
#include "string16.h"

namespace base
{
    inline bool IsValidCodepoint(uint32 code_point)
    {
        return code_point < 0xD800u ||
            (code_point >= 0xE000u && code_point <= 0x10FFFFu);
    }

    inline bool IsValidCharacter(uint32 code_point)
    {
        return code_point < 0xD800u || (code_point >= 0xE000u &&
            code_point < 0xFDD0u) || (code_point > 0xFDEFu &&
                code_point <= 0x10FFFFu && (code_point & 0xFFFEu) != 0xFFFEu);
    }

    bool ReadUnicodeCharacter(const char* src, int32 src_len,
        int32* char_index, uint32* code_point);

    bool ReadUnicodeCharacter(const char16* src, int32 src_len,
        int32* char_index, uint32* code_point);

    size_t WriteUnicodeCharacter(uint32 code_point, std::string* output);

    size_t WriteUnicodeCharacter(uint32 code_point, string16* output);

    template<typename CHAR>
    void PrepareForUTF8Output(const CHAR* src, size_t src_len, std::string* output);

    template<typename STRING>
    void PrepareForUTF16Or32Output(const char* src, size_t src_len, STRING* output);

} //namespace base

#endif //__base_utf_string_conversion_utils_h__