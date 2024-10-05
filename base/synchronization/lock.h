#ifndef __base_lock_h__
#define __base_lock_h__

#include "base/threading/platform_thread.h"
#include "lock_impl.h"

namespace base
{
    class Lock
    {
    public:
#if defined(NDEBUG)
        Lock() : lock_() {}
        ~Lock() {}
        void Acquire() { lock_.Lock(); }
        void Release() { lock_.Unlock(); }

        bool Try() { return lock_.Try(); }

        void AssertAcquired() const {}
#else //!NDEBUG
        Lock();
        ~Lock() {}

        void Acquire()
        {
            lock_.Lock();
            CheckUnheldAndMark();
        }
        void Release()
        {
            CheckHeldAndUnmark();
            lock_.Unlock();
        }

        bool Try()
        {
            bool rv = lock_.Try();
            if (rv)
            {
                CheckUnheldAndMark();
            }
            return rv;
        }

        void AssertAcquired() const;
#endif //NDEBUG

    private:
#if !defined(NDEBUG)
        void CheckHeldAndUnmark();
        void CheckUnheldAndMark();

        bool owned_by_thread_;
        base::PlatformThreadId owning_thread_id_;
#endif //NDEBUG

        internal::LockImpl lock_;

        DISALLOW_COPY_AND_ASSIGN(Lock);
    };

    class AutoLock
    {
    public:
        explicit AutoLock(Lock& lock) : lock_(lock)
        {
            lock_.Acquire();
        }

        ~AutoLock()
        {
            lock_.AssertAcquired();
            lock_.Release();
        }

    private:
        Lock& lock_;
        DISALLOW_COPY_AND_ASSIGN(AutoLock);
    };

    class AutoUnlock
    {
    public:
        explicit AutoUnlock(Lock& lock) : lock_(lock)
        {
            lock_.AssertAcquired();
            lock_.Release();
        }

        ~AutoUnlock()
        {
            lock_.Acquire();
        }

    private:
        Lock& lock_;
        DISALLOW_COPY_AND_ASSIGN(AutoUnlock);
    };

} //namespace base

#endif //__base_lock_h__