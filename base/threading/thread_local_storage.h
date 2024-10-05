#ifndef __base_thread_local_storage_h__
#define __base_thread_local_storage_h__

#include "base/basic_types.h"

namespace base
{
    class ThreadLocalStorage
    {
    public:
        typedef void (*TLSDestructorFunc)(void* value);

        class Slot
        {
        public:
            explicit Slot(TLSDestructorFunc destructor = NULL);

            explicit Slot(base::LinkerInitialized x) {}

            bool Initialize(TLSDestructorFunc destructor);

            void Free();

            void* Get() const;

            void Set(void* value);

            bool initialized() const { return initialized_; }

        private:
            bool initialized_;
            int slot_;

            DISALLOW_COPY_AND_ASSIGN(Slot);
        };

        static void ThreadExit();

    private:
        static void** Initialize();

    private:
        static const int kThreadLocalStorageSize = 64;

        static long tls_key_;
        static long tls_max_;
        static TLSDestructorFunc tls_destructors_[kThreadLocalStorageSize];

        DISALLOW_COPY_AND_ASSIGN(ThreadLocalStorage);
    };

} //namespace base

#endif //__base_thread_local_storage_h__