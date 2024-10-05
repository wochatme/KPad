#ifndef __base_wrapped_window_proc_h__
#define __base_wrapped_window_proc_h__

#include <windows.h>

namespace base
{
    namespace win
    {
        typedef int(__cdecl* WinProcExceptionFilter)(EXCEPTION_POINTERS* info);

        WinProcExceptionFilter SetWinProcExceptionFilter(WinProcExceptionFilter filter);

        int CallExceptionFilter(EXCEPTION_POINTERS* info);

        //
        // LRESULT CALLBACK MyWinProc(HWND hwnd, UINT message,
        //                            WPARAM wparam, LPARAM lparam) {
        //   // Do Something.
        // }
        //
        // ...
        //
        //   WNDCLASSEX wc = { 0 };
        //   wc.lpfnWndProc = WrappedWindowProc<MyWinProc>;
        //   wc.lpszClassName = class_name;
        //   ...
        //   RegisterClassEx(&wc);
        //
        //   CreateWindowW(class_name, window_name, ...
        template<WNDPROC proc>
        LRESULT CALLBACK WrappedWindowProc(HWND hwnd, UINT message,
            WPARAM wparam, LPARAM lparam)
        {
            LRESULT rv = 0;
            __try
            {
                rv = proc(hwnd, message, wparam, lparam);
            }
            __except (CallExceptionFilter(GetExceptionInformation()))
            {
            }
            return rv;
        }

    } //namespace win
} //namespace base

#endif //__base_wrapped_window_proc_h__