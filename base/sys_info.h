#ifndef __base_sys_info_h__
#define __base_sys_info_h__

#include <string>

#include "basic_types.h"

class FilePath;

namespace base
{
    class SysInfo
    {
    public:
        static int NumberOfProcessors();

        static int64 AmountOfPhysicalMemory();

        static int AmountOfPhysicalMemoryMB()
        {
            return static_cast<int>(AmountOfPhysicalMemory() / 1024 / 1024);
        }

        static int64 AmountOfFreeDiskSpace(const FilePath& path);

        static std::string OperatingSystemName();

        static std::string OperatingSystemVersion();

        static void OperatingSystemVersionNumbers(int32* major_version,
            int32* minor_version, int32* bugfix_version);

        static std::string CPUArchitecture();

        static void GetPrimaryDisplayDimensions(int* width, int* height);

        static int DisplayCount();

        static size_t VMAllocationGranularity();
    };

} //namespace base

#endif //__base_sys_info_h__