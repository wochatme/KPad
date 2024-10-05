#ifndef __base_platform_thread_h__
#define __base_platform_thread_h__

#include <windows.h>
#include "base/basic_types.h"

namespace base
{
    // PlatformThreadHandle should not be assumed to be a numeric type, since the
    // standard intends to allow pthread_t to be a structure.  This means you
    // should not initialize it to a value, like 0.  If it's a member variable, the
    // constructor can safely "value initialize" using () in the initializer list.
    typedef DWORD PlatformThreadId;
    typedef void* PlatformThreadHandle; // HANDLE
    const PlatformThreadHandle kNullThreadHandle = NULL;

    const PlatformThreadId kInvalidThreadId = 0;

    // Valid values for SetThreadPriority()
    enum ThreadPriority
    {
        kThreadPriority_Normal,
        // Suitable for low-latency, glitch-resistant audio.
        kThreadPriority_RealtimeAudio
    };

    // A namespace for low-level thread functions.
    class PlatformThread
    {
    public:
        class Delegate
        {
        public:
            virtual ~Delegate() {}
            virtual void ThreadMain() = 0;
        };

        // Gets the current thread id, which may be useful for logging purposes.
        static PlatformThreadId CurrentId();

        // Yield the current thread so another thread can be scheduled.
        static void YieldCurrentThread();

        // Sleeps for the specified duration (units are milliseconds).
        static void Sleep(int duration_ms);

        static void SetName(const char* name);

        // Gets the thread name, if previously set by SetName.
        static const char* GetName();

        static bool Create(size_t stack_size, Delegate* delegate,
            PlatformThreadHandle* thread_handle);

        // CreateNonJoinable() does the same thing as Create() except the thread
        // cannot be Join()'d.  Therefore, it also does not output a
        // PlatformThreadHandle.
        static bool CreateNonJoinable(size_t stack_size, Delegate* delegate);

        static void Join(PlatformThreadHandle thread_handle);

        static void SetThreadPriority(PlatformThreadHandle handle,
            ThreadPriority priority);

    private:
        DISALLOW_IMPLICIT_CONSTRUCTORS(PlatformThread);
    };

} //namespace base

#endif //__base_platform_thread_h__