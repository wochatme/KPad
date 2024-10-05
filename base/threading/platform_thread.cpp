#include "platform_thread.h"

#include "base/logging.h"
#include "base/win/windows_version.h"
#include "thread_local.h"
#include "thread_restrictions.h"

namespace base
{
    namespace
    {
        static ThreadLocalPointer<char> current_thread_name;

        //   http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
        const DWORD kVCThreadNameException = 0x406D1388;

        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType;     
            LPCSTR szName;    
            DWORD dwThreadID; 
            DWORD dwFlags;    
        } THREADNAME_INFO;

        struct ThreadParams
        {
            PlatformThread::Delegate* delegate;
            bool joinable;
        };

        DWORD __stdcall ThreadFunc(void* params)
        {
            ThreadParams* thread_params = static_cast<ThreadParams*>(params);
            PlatformThread::Delegate* delegate = thread_params->delegate;
            if (!thread_params->joinable)
            {
                ThreadRestrictions::SetSingletonAllowed(false);
            }
            delete thread_params;
            delegate->ThreadMain();
            return NULL;
        }

        bool CreateThreadInternal(size_t stack_size,
            PlatformThread::Delegate* delegate,
            PlatformThreadHandle* out_thread_handle)
        {
            PlatformThreadHandle thread_handle;
            unsigned int flags = 0;
            if (stack_size > 0 && win::GetVersion() >= win::VERSION_XP)
            {
                flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
            }
            else
            {
                stack_size = 0;
            }

            ThreadParams* params = new ThreadParams;
            params->delegate = delegate;
            params->joinable = out_thread_handle != NULL;

            //   http://www.microsoft.com/msj/1099/win32/win321099.aspx
            thread_handle = CreateThread(NULL, stack_size, ThreadFunc, params,
                flags, NULL);
            if (!thread_handle)
            {
                delete params;
                return false;
            }

            if (out_thread_handle)
            {
                *out_thread_handle = thread_handle;
            }
            else
            {
                CloseHandle(thread_handle);
            }
            return true;
        }

    }

    // static
    PlatformThreadId PlatformThread::CurrentId()
    {
        return GetCurrentThreadId();
    }

    // static
    void PlatformThread::YieldCurrentThread()
    {
        ::Sleep(0);
    }

    // static
    void PlatformThread::Sleep(int duration_ms)
    {
        ::Sleep(duration_ms);
    }

    // static
    void PlatformThread::SetName(const char* name)
    {
        current_thread_name.Set(const_cast<char*>(name));

        if (!::IsDebuggerPresent())
        {
            return;
        }

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = CurrentId();
        info.dwFlags = 0;

        __try
        {
            RaiseException(kVCThreadNameException, 0, sizeof(info) / sizeof(DWORD),
                reinterpret_cast<DWORD_PTR*>(&info));
        }
        __except (EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }

    // static
    const char* PlatformThread::GetName()
    {
        return current_thread_name.Get();
    }

    // static
    bool PlatformThread::Create(size_t stack_size, Delegate* delegate,
        PlatformThreadHandle* thread_handle)
    {
        DCHECK(thread_handle);
        return CreateThreadInternal(stack_size, delegate, thread_handle);
    }

    // static
    bool PlatformThread::CreateNonJoinable(size_t stack_size, Delegate* delegate)
    {
        return CreateThreadInternal(stack_size, delegate, NULL);
    }

    // static
    void PlatformThread::Join(PlatformThreadHandle thread_handle)
    {
        DCHECK(thread_handle);

#if 0
        ThreadRestrictions::AssertIOAllowed();
#endif

        DWORD result = WaitForSingleObject(thread_handle, INFINITE);
        DCHECK_EQ(WAIT_OBJECT_0, result);

        CloseHandle(thread_handle);
    }

    // static
    void PlatformThread::SetThreadPriority(PlatformThreadHandle, ThreadPriority)
    {
        // TODO(crogers): implement
        NOTIMPLEMENTED();
    }

} //namespace base