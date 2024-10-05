#ifndef __base_threading_thread_restrictions_h__
#define __base_threading_thread_restrictions_h__

#include "base/basic_types.h"

namespace base
{
    class ThreadRestrictions
    {
    public:
        class ScopedAllowIO
        {
        public:
            ScopedAllowIO() { previous_value_ = SetIOAllowed(true); }
            ~ScopedAllowIO() { SetIOAllowed(previous_value_); }
        private:
            bool previous_value_;

            DISALLOW_COPY_AND_ASSIGN(ScopedAllowIO);
        };

        class ScopedAllowSingleton
        {
        public:
            ScopedAllowSingleton() { previous_value_ = SetSingletonAllowed(true); }
            ~ScopedAllowSingleton() { SetSingletonAllowed(previous_value_); }
        private:
            bool previous_value_;

            DISALLOW_COPY_AND_ASSIGN(ScopedAllowSingleton);
        };

#ifndef NDEBUG
        static bool SetIOAllowed(bool allowed);

        static void AssertIOAllowed();

        static bool SetSingletonAllowed(bool allowed);

        static void AssertSingletonAllowed();
#else
        static bool SetIOAllowed(bool allowed) { return true; }
        static void AssertIOAllowed() {}
        static bool SetSingletonAllowed(bool allowed) { return true; }
        static void AssertSingletonAllowed() {}
#endif

    private:
        DISALLOW_IMPLICIT_CONSTRUCTORS(ThreadRestrictions);
    };

} //namespace base

#endif //__base_threading_thread_restrictions_h__