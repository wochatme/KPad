#ifndef __base_lock_impl_h__
#define __base_lock_impl_h__

#include <windows.h>

#include "base/basic_types.h"

namespace base
{
    namespace internal
    {
        class LockImpl
        {
        public:
            typedef CRITICAL_SECTION OSLockType;

            LockImpl();
            ~LockImpl();

            bool Try();
            void Lock();
            void Unlock();

        private:
            OSLockType os_lock_;

            DISALLOW_COPY_AND_ASSIGN(LockImpl);
        };

    } //namespace internal
} //namespace base

#endif //__base_lock_impl_h__