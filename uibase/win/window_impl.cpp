#if 0
#include "window_impl.h"

#include <list>
#include "base/memory/singleton.h"
#include "base/string_number_conversions.h"
#include "base/win/wrapped_window_proc.h"

#include "hwnd_util.h"

namespace ui
{
    static const DWORD kWindowDefaultChildStyle =
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    static const DWORD kWindowDefaultStyle = WS_OVERLAPPEDWINDOW;
    static const DWORD kWindowDefaultExStyle = 0;

    ///////////////////////////////////////////////////////////////////////////////
    const wchar_t* const WindowImpl::kBaseClassName = L"Wan_WidgetWin_";

    struct ClassInfo
    {
        UINT style;
        HBRUSH background;

        explicit ClassInfo(int style) : style(style), background(NULL) {}

        bool Equals(const ClassInfo& other) const
        {
            return (other.style == style && other.background == background);
        }
    };

    class ClassRegistrar
    {
    public:
        static ClassRegistrar* GetInstance()
        {
            return Singleton<ClassRegistrar>::get();
        }

        ~ClassRegistrar()
        {
            for (RegisteredClasses::iterator i = registered_classes_.begin();
                i != registered_classes_.end(); ++i)
            {
                UnregisterClass(i->name.c_str(), NULL);
            }
        }

        bool RetrieveClassName(const ClassInfo& class_info, std::wstring* name)
        {
            for (RegisteredClasses::const_iterator i = registered_classes_.begin();
                i != registered_classes_.end(); ++i)
            {
                if (class_info.Equals(i->info))
                {
                    name->assign(i->name);
                    return true;
                }
            }

            name->assign(string16(WindowImpl::kBaseClassName) +
                base::IntToString16(registered_count_++));
            return false;
        }

        void RegisterClass(const ClassInfo& class_info,
            const std::wstring& name, ATOM atom)
        {
            registered_classes_.push_back(RegisteredClass(class_info, name, atom));
        }

    private:
        struct RegisteredClass
        {
            RegisteredClass(const ClassInfo& info, const std::wstring& name,
                ATOM atom) : info(info), name(name), atom(atom) {}

            ClassInfo info;

            std::wstring name;

            ATOM atom;
        };

        ClassRegistrar() : registered_count_(0) {}
        friend struct DefaultSingletonTraits<ClassRegistrar>;

        typedef std::list<RegisteredClass> RegisteredClasses;
        RegisteredClasses registered_classes_;

        int registered_count_;

        DISALLOW_COPY_AND_ASSIGN(ClassRegistrar);
    };


    WindowImpl::WindowImpl()
        : window_style_(0),
        window_ex_style_(kWindowDefaultExStyle),
        class_style_(CS_DBLCLKS),
        hwnd_(NULL) {}

    WindowImpl::~WindowImpl() {}

    void WindowImpl::Init(HWND parent, const gfx::Rect& bounds)
    {
        if (window_style_ == 0)
        {
            window_style_ = parent ? kWindowDefaultChildStyle : kWindowDefaultStyle;
        }

        if (parent && !::IsWindow(parent))
        {
            NOTREACHED() << "invalid parent window specified.";
            parent = NULL;
        }

        int x, y, width, height;
        if (bounds.IsEmpty())
        {
            x = y = width = height = CW_USEDEFAULT;
        }
        else
        {
            x = bounds.x();
            y = bounds.y();
            width = bounds.width();
            height = bounds.height();
        }

        std::wstring name(GetWindowClassName());
        hwnd_ = CreateWindowEx(window_ex_style_, name.c_str(), NULL,
            window_style_, x, y, width, height, parent, NULL, NULL, this);
        CheckWindowCreated(hwnd_);

        CHECK_EQ(GetWindowUserData(hwnd_), this);
    }

    HICON WindowImpl::GetDefaultWindowIcon() const
    {
        return NULL;
    }

    LRESULT WindowImpl::OnWndProc(UINT message, WPARAM w_param, LPARAM l_param)
    {
        LRESULT result = 0;

        if (!ProcessWindowMessage(hwnd_, message, w_param, l_param, result))
        {
            result = DefWindowProc(hwnd_, message, w_param, l_param);
        }

        return result;
    }

    // static
    LRESULT CALLBACK WindowImpl::WndProc(HWND hwnd,
        UINT message, WPARAM w_param, LPARAM l_param)
    {
        if (message == WM_NCCREATE)
        {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(l_param);
            WindowImpl* window = reinterpret_cast<WindowImpl*>(cs->lpCreateParams);
            DCHECK(window);
            SetWindowUserData(hwnd, window);
            window->hwnd_ = hwnd;
            return TRUE;
        }

        WindowImpl* window = reinterpret_cast<WindowImpl*>(
            GetWindowUserData(hwnd));
        if (!window)
        {
            return 0;
        }

        return window->OnWndProc(message, w_param, l_param);
    }

    std::wstring WindowImpl::GetWindowClassName()
    {
        ClassInfo class_info(initial_class_style());
        std::wstring name;
        if (ClassRegistrar::GetInstance()->RetrieveClassName(class_info, &name))
        {
            return name;
        }

        WNDCLASSEX class_ex;
        class_ex.cbSize = sizeof(WNDCLASSEX);
        class_ex.style = class_info.style;
        class_ex.lpfnWndProc = base::win::WrappedWindowProc<&WindowImpl::WndProc>;
        class_ex.cbClsExtra = 0;
        class_ex.cbWndExtra = 0;
        class_ex.hInstance = NULL;
        class_ex.hIcon = GetDefaultWindowIcon();
        class_ex.hCursor = LoadCursor(NULL, IDC_ARROW);
        class_ex.hbrBackground = reinterpret_cast<HBRUSH>(class_info.background + 1);
        class_ex.lpszMenuName = NULL;
        class_ex.lpszClassName = name.c_str();
        class_ex.hIconSm = class_ex.hIcon;
        ATOM atom = RegisterClassEx(&class_ex);
        DCHECK(atom);

        ClassRegistrar::GetInstance()->RegisterClass(class_info, name, atom);

        return name;
    }

} //namespace ui

#endif 

