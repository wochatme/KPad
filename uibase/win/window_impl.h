#ifndef __ui_base_window_impl_h__
#define __ui_base_window_impl_h__

#include <windows.h>
#include <string>

#include "base/logging.h"
#include "uigfx/rect.h"

namespace ui
{
    class MessageMapInterface
    {
    public:
        virtual BOOL ProcessWindowMessage(HWND window,
            UINT message,
            WPARAM w_param,
            LPARAM l_param,
            LRESULT& result,
            DWORD msg_mad_id = 0) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    // WindowImpl
    //
    ///////////////////////////////////////////////////////////////////////////////
    class WindowImpl : public MessageMapInterface
    {
    public:
        WindowImpl();
        virtual ~WindowImpl();

        void Init(HWND parent, const gfx::Rect& bounds);

        virtual HICON GetDefaultWindowIcon() const;

        HWND hwnd() const { return hwnd_; }

        void set_window_style(DWORD style) { window_style_ = style; }
        DWORD window_style() const { return window_style_; }

        void set_window_ex_style(DWORD style) { window_ex_style_ = style; }
        DWORD window_ex_style() const { return window_ex_style_; }

        void set_initial_class_style(UINT class_style)
        {
            DCHECK_EQ((class_style & CS_GLOBALCLASS), 0u);
            class_style_ = class_style;
        }
        UINT initial_class_style() const { return class_style_; }

    protected:
        virtual LRESULT OnWndProc(UINT message, WPARAM w_param, LPARAM l_param);

    private:
        friend class ClassRegistrar;

        static LRESULT CALLBACK WndProc(HWND window, UINT message,
            WPARAM w_param, LPARAM l_param);

        std::wstring GetWindowClassName();

        static const wchar_t* const kBaseClassName;

        DWORD window_style_;

        DWORD window_ex_style_;

        UINT class_style_;

        HWND hwnd_;

        DISALLOW_COPY_AND_ASSIGN(WindowImpl);
    };

} //namespace ui

#endif //__ui_base_window_impl_h__