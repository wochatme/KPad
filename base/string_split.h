#ifndef __base_string_split_h__
#define __base_string_split_h__

#include <vector>
#include "string16.h"

namespace base
{
    void SplitString(const std::wstring& str,
        wchar_t c, std::vector<std::wstring>* r);

    void SplitString(const std::string& str,
        char16 c, std::vector<std::string>* r);

    void SplitString(const std::string& str,
        char c, std::vector<std::string>* r);

    bool SplitStringIntoKeyValues(const std::string& line,
        char key_value_delimiter, std::string* key,
        std::vector<std::string>* values);

    bool SplitStringIntoKeyValuePairs(const std::string& line,
        char key_value_delimiter, char key_value_pair_delimiter,
        std::vector<std::pair<std::string, std::string> >* kv_pairs);

    void SplitStringUsingSubstr(const string16& str,
        const string16& s, std::vector<string16>* r);
    void SplitStringUsingSubstr(const std::string& str,
        const std::string& s, std::vector<std::string>* r);

    void SplitStringDontTrim(const string16& str,
        char16 c, std::vector<string16>* r);

    void SplitStringDontTrim(const std::string& str,
        char c, std::vector<std::string>* r);

} //namespace base

#endif //__base_string_split_h__