#ifndef __base_string_util_h__
#define __base_string_util_h__

#include <ctype.h>
#include <stdarg.h> 

#include <string>
#include <vector>

#include "logging.h"
#include "string16.h"
#include "string_piece.h"

namespace base
{

    int strcasecmp(const char* s1, const char* s2);

    int strncasecmp(const char* s1, const char* s2, size_t count);

    int strncmp16(const char16* s1, const char16* s2, size_t count);

    int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);

    int vswprintf(wchar_t* buffer, size_t size, const wchar_t* format,
        va_list arguments);

    inline int snprintf(char* buffer, size_t size, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        int result = vsnprintf(buffer, size, format, arguments);
        va_end(arguments);
        return result;
    }

    inline int swprintf(wchar_t* buffer, size_t size, const wchar_t* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        int result = vswprintf(buffer, size, format, arguments);
        va_end(arguments);
        return result;
    }

    size_t strlcpy(char* dst, const char* src, size_t dst_size);
    size_t wcslcpy(wchar_t* dst, const wchar_t* src, size_t dst_size);

    bool IsWprintfFormatPortable(const wchar_t* format);

    inline char* strdup(const char* str)
    {
        return _strdup(str);
    }

    inline int strcasecmp(const char* s1, const char* s2)
    {
        return _stricmp(s1, s2);
    }

    inline int strncasecmp(const char* s1, const char* s2, size_t count)
    {
        return _strnicmp(s1, s2, count);
    }

    inline int strncmp16(const char16* s1, const char16* s2, size_t count)
    {
        return ::wcsncmp(s1, s2, count);
    }

    inline int vsnprintf(char* buffer, size_t size, const char* format,
        va_list arguments)
    {
        int length = vsnprintf_s(buffer, size, size - 1, format, arguments);
        if (length < 0)
        {
            return _vscprintf(format, arguments);
        }
        return length;
    }

    inline int vswprintf(wchar_t* buffer, size_t size, const wchar_t* format,
        va_list arguments)
    {
        DCHECK(IsWprintfFormatPortable(format));

        int length = _vsnwprintf_s(buffer, size, size - 1, format, arguments);
        if (length < 0)
        {
            return _vscwprintf(format, arguments);
        }
        return length;
    }

} // namespace base

const std::string& EmptyString();
const std::wstring& EmptyWString();
const string16& EmptyString16();

extern const wchar_t kWhitespaceWide[];
extern const char16 kWhitespaceUTF16[];
extern const char kWhitespaceASCII[];

extern const char kUtf8ByteOrderMark[];

bool RemoveChars(const std::wstring& input, const wchar_t remove_chars[],
    std::wstring* output);
bool RemoveChars(const string16& input, const char16 remove_chars[],
    string16* output);
bool RemoveChars(const std::string& input, const char remove_chars[],
    std::string* output);

bool TrimString(const std::wstring& input, const wchar_t trim_chars[],
    std::wstring* output);
bool TrimString(const string16& input, const char16 trim_chars[],
    string16* output);
bool TrimString(const std::string& input, const char trim_chars[],
    std::string* output);

void TruncateUTF8ToByteSize(const std::string& input, const size_t byte_size,
    std::string* output);

enum TrimPositions
{
    TRIM_NONE = 0,
    TRIM_LEADING = 1 << 0,
    TRIM_TRAILING = 1 << 1,
    TRIM_ALL = TRIM_LEADING | TRIM_TRAILING,
};
TrimPositions TrimWhitespace(const std::wstring& input, TrimPositions positions,
    std::wstring* output);
TrimPositions TrimWhitespace(const string16& input, TrimPositions positions,
    string16* output);
TrimPositions TrimWhitespace(const std::string& input, TrimPositions positions,
    std::string* output);
TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions,
    std::string* output);

std::wstring CollapseWhitespace(const std::wstring& text,
    bool trim_sequences_with_line_breaks);
string16 CollapseWhitespace(const string16& text,
    bool trim_sequences_with_line_breaks);
std::string CollapseWhitespaceASCII(const std::string& text,
    bool trim_sequences_with_line_breaks);

bool ContainsOnlyWhitespaceASCII(const std::string& str);
bool ContainsOnlyWhitespace(const string16& str);

bool ContainsOnlyChars(const std::wstring& input, const std::wstring& characters);
bool ContainsOnlyChars(const string16& input, const string16& characters);
bool ContainsOnlyChars(const std::string& input, const std::string& characters);

std::string WideToASCII(const std::wstring& wide);
std::string UTF16ToASCII(const string16& utf16);

bool WideToLatin1(const std::wstring& wide, std::string* latin1);

bool IsStringUTF8(const std::string& str);
bool IsStringASCII(const std::wstring& str);
bool IsStringASCII(const base::StringPiece& str);
bool IsStringASCII(const string16& str);

template<class Char>
inline Char ToLowerASCII(Char c)
{
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

template<class str>
inline void StringToLowerASCII(str* s)
{
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
    {
        *i = ToLowerASCII(*i);
    }
}

template<class str>
inline str StringToLowerASCII(const str& s)
{
    str output(s);
    StringToLowerASCII(&output);
    return output;
}

template<class Char>
inline Char ToUpperASCII(Char c)
{
    return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

template<class str>
inline void StringToUpperASCII(str* s)
{
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
    {
        *i = ToUpperASCII(*i);
    }
}

template<class str>
inline str StringToUpperASCII(const str& s)
{
    str output(s);
    StringToUpperASCII(&output);
    return output;
}

bool LowerCaseEqualsASCII(const std::string& a, const char* b);
bool LowerCaseEqualsASCII(const std::wstring& a, const char* b);
bool LowerCaseEqualsASCII(const string16& a, const char* b);

bool LowerCaseEqualsASCII(std::string::const_iterator a_begin,
    std::string::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(std::wstring::const_iterator a_begin,
    std::wstring::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(string16::const_iterator a_begin,
    string16::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(const char* a_begin, const char* a_end, const char* b);
bool LowerCaseEqualsASCII(const wchar_t* a_begin, const wchar_t* a_end,
    const char* b);
bool LowerCaseEqualsASCII(const char16* a_begin, const char16* a_end, const char* b);

bool EqualsASCII(const string16& a, const base::StringPiece& b);

bool StartsWithASCII(const std::string& str, const std::string& search,
    bool case_sensitive);
bool StartsWith(const std::wstring& str, const std::wstring& search,
    bool case_sensitive);
bool StartsWith(const string16& str, const string16& search,
    bool case_sensitive);

bool EndsWith(const std::string& str, const std::string& search,
    bool case_sensitive);
bool EndsWith(const std::wstring& str, const std::wstring& search,
    bool case_sensitive);
bool EndsWith(const string16& str, const string16& search,
    bool case_sensitive);

template<typename Char>
inline bool IsAsciiWhitespace(Char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}
template<typename Char>
inline bool IsAsciiAlpha(Char c)
{
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}
template<typename Char>
inline bool IsAsciiDigit(Char c)
{
    return c >= '0' && c <= '9';
}

template<typename Char>
inline bool IsHexDigit(Char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

template<typename Char>
inline Char HexDigitToInt(Char c)
{
    DCHECK(IsHexDigit(c));
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    return 0;
}

inline bool IsWhitespace(wchar_t c)
{
    return wcschr(kWhitespaceWide, c) != NULL;
}

// Return a byte string in human-readable format with a unit suffix. Not
// appropriate for use in any UI; use of FormatBytes and friends in ui/base is
// highly recommended instead. TODO(avi): Figure out how to get callers to use
// FormatBytes instead; remove this.
string16 FormatBytesUnlocalized(int64 bytes);

void ReplaceFirstSubstringAfterOffset(string16* str,
    string16::size_type start_offset,
    const string16& find_this,
    const string16& replace_with);
void ReplaceFirstSubstringAfterOffset(std::string* str,
    std::string::size_type start_offset,
    const std::string& find_this,
    const std::string& replace_with);

void ReplaceSubstringsAfterOffset(string16* str,
    string16::size_type start_offset,
    const string16& find_this,
    const string16& replace_with);
void ReplaceSubstringsAfterOffset(std::string* str,
    std::string::size_type start_offset,
    const std::string& find_this,
    const std::string& replace_with);

template<class string_type>
inline typename string_type::value_type* WriteInto(string_type* str,
    size_t length_with_null)
{
    str->reserve(length_with_null);
    str->resize(length_with_null - 1);
    return length_with_null <= 1 ? NULL : &((*str)[0]);
}

template<typename Char> struct CaseInsensitiveCompare
{
public:
    bool operator()(Char x, Char y) const
    {
        return tolower(x) == tolower(y);
    }
};

template<typename Char> struct CaseInsensitiveCompareASCII
{
public:
    bool operator()(Char x, Char y) const
    {
        return ToLowerASCII(x) == ToLowerASCII(y);
    }
};

size_t Tokenize(const std::wstring& str, const std::wstring& delimiters,
    std::vector<std::wstring>* tokens);
size_t Tokenize(const string16& str, const string16& delimiters,
    std::vector<string16>* tokens);
size_t Tokenize(const std::string& str, const std::string& delimiters,
    std::vector<std::string>* tokens);
size_t Tokenize(const base::StringPiece& str,
    const base::StringPiece& delimiters,
    std::vector<base::StringPiece>* tokens);

string16 JoinString(const std::vector<string16>& parts, char16 s);
std::string JoinString(const std::vector<std::string>& parts, char s);

void SplitStringAlongWhitespace(const std::wstring& str,
    std::vector<std::wstring>* result);
void SplitStringAlongWhitespace(const string16& str,
    std::vector<string16>* result);
void SplitStringAlongWhitespace(const std::string& str,
    std::vector<std::string>* result);

string16 ReplaceStringPlaceholders(const string16& format_string,
    const std::vector<string16>& subst,
    std::vector<size_t>* offsets);

std::string ReplaceStringPlaceholders(const base::StringPiece& format_string,
    const std::vector<std::string>& subst,
    std::vector<size_t>* offsets);

string16 ReplaceStringPlaceholders(const string16& format_string,
    const string16& a,
    size_t* offset);

bool MatchPattern(const base::StringPiece& string,
    const base::StringPiece& pattern);
bool MatchPattern(const string16& string, const string16& pattern);

template<typename T>
struct ToUnsigned
{
    typedef T Unsigned;
};

template<>
struct ToUnsigned<char>
{
    typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<signed char>
{
    typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<wchar_t>
{
    typedef unsigned short Unsigned;
};
template<>
struct ToUnsigned<short>
{
    typedef unsigned short Unsigned;
};

#endif //__base_string_util_h__