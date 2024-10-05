#ifndef __base_string_number_conversions_h__
#define __base_string_number_conversions_h__

#include <string>
#include <vector>

#include "basic_types.h"
#include "string16.h"

namespace base
{
    std::string IntToString(int value);
    string16 IntToString16(int value);

    std::string UintToString(unsigned value);
    string16 UintToString16(unsigned value);

    std::string Int64ToString(int64 value);
    string16 Int64ToString16(int64 value);

    std::string Uint64ToString(uint64 value);
    string16 Uint64ToString16(uint64 value);

    std::string DoubleToString(double value);

    bool StringToInt(const std::string& input, int* output);
    bool StringToInt(std::string::const_iterator begin,
        std::string::const_iterator end, int* output);
    bool StringToInt(const char* begin, const char* end, int* output);

    bool StringToInt(const string16& input, int* output);
    bool StringToInt(string16::const_iterator begin,
        string16::const_iterator end, int* output);
    bool StringToInt(const char16* begin, const char16* end, int* output);

    bool StringToInt64(const std::string& input, int64* output);
    bool StringToInt64(std::string::const_iterator begin,
        std::string::const_iterator end, int64* output);
    bool StringToInt64(const char* begin, const char* end, int64* output);

    bool StringToInt64(const string16& input, int64* output);
    bool StringToInt64(string16::const_iterator begin,
        string16::const_iterator end, int64* output);
    bool StringToInt64(const char16* begin, const char16* end, int64* output);


    bool StringToDouble(const std::string& input, double* output);

    std::string HexEncode(const void* bytes, size_t size);

    bool HexStringToInt(const std::string& input, int* output);
    bool HexStringToInt(std::string::const_iterator begin,
        std::string::const_iterator end, int* output);
    bool HexStringToInt(const char* begin, const char* end, int* output);

    bool HexStringToBytes(const std::string& input, std::vector<uint8>* output);

} //namespace base

#endif //__base_string_number_conversions_h__