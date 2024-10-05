#ifndef __base_windows_version_h__
#define __base_windows_version_h__

#include "base/basic_types.h"

typedef void* HANDLE;

namespace base
{
    namespace win
    {
        // "if(base::win::GetVersion() >= base::win::VERSION_VISTA) ...".
        enum Version
        {
            VERSION_PRE_XP = 0, // does not support
            VERSION_XP,
            VERSION_SERVER_2003, // Also includes Windows XP Professional x64.
            VERSION_VISTA,
            VERSION_SERVER_2008,
            VERSION_WIN7,
        };

        // A singleton that can be used to query various pieces of information about the
        // OS and process state. Note that this doesn't use the base Singleton class, so
        // it can be used without an AtExitManager.
        class OSInfo
        {
        public:
            struct VersionNumber
            {
                int major;
                int minor;
                int build;
            };

            struct ServicePack
            {
                int major;
                int minor;
            };

            enum WindowsArchitecture
            {
                X86_ARCHITECTURE,
                X64_ARCHITECTURE,
                IA64_ARCHITECTURE,
                OTHER_ARCHITECTURE,
            };

            enum WOW64Status
            {
                WOW64_DISABLED,
                WOW64_ENABLED,
                WOW64_UNKNOWN,
            };

            static OSInfo* GetInstance();

            Version version() const { return version_; }
            // The next two functions return arrays of values, [major, minor(, build)].
            VersionNumber version_number() const { return version_number_; }
            ServicePack service_pack() const { return service_pack_; }
            WindowsArchitecture architecture() const { return architecture_; }
            int processors() const { return processors_; }
            size_t allocation_granularity() const { return allocation_granularity_; }
            WOW64Status wow64_status() const { return wow64_status_; }

            static WOW64Status GetWOW64StatusForProcess(HANDLE process_handle);

        private:
            OSInfo();
            ~OSInfo();

            Version version_;
            VersionNumber version_number_;
            ServicePack service_pack_;
            WindowsArchitecture architecture_;
            int processors_;
            size_t allocation_granularity_;
            WOW64Status wow64_status_;

            DISALLOW_COPY_AND_ASSIGN(OSInfo);
        };

        Version GetVersion();

    } //namespace win
} //namespace base

#endif //__base_windows_version_h__