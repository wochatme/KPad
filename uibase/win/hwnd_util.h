
#ifndef __ui_base_hwnd_util_h__
#define __ui_base_hwnd_util_h__

#include <windows.h>
#include <string>

namespace gfx
{
    class Size;
}

namespace ui
{
    std::wstring GetClassName(HWND window);

    WNDPROC SetWindowProc(HWND hwnd, WNDPROC wndproc);

    void* SetWindowUserData(HWND hwnd, void* user_data);
    void* GetWindowUserData(HWND hwnd);

    // Returns true if the specified window is the current active top window or one
    // of its children.
    bool DoesWindowBelongToActiveWindow(HWND window);

    // Sizes the window to have a client or window size (depending on the value of
    // |pref_is_client|) of pref, then centers the window over parent, ensuring the
    // window fits on screen.
    void CenterAndSizeWindow(HWND parent, HWND window,
        const gfx::Size& pref, bool pref_is_client);

    // If |hwnd| is NULL logs various thing and CHECKs. Invoke right after calling
    // CreateWindow.
    void CheckWindowCreated(HWND hwnd);

    // Shows the system menu for |window| and sends the selected command (if the
    // user selected something.
    void ShowSystemMenu(HWND window, int screen_x, int screen_y);

} //namespace ui

#endif //__ui_base_hwnd_util_h__

