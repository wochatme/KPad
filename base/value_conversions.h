#ifndef __base_value_conversionss_h__
#define __base_value_conversionss_h__

class FilePath;

namespace base
{
    class StringValue;
    class Value;

    StringValue* CreateFilePathValue(const FilePath& in_value);
    bool GetValueAsFilePath(const Value& value, FilePath* file_path);
}

#endif //__base_value_conversionss_h__