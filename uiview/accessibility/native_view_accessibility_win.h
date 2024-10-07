#ifndef __view_native_view_accessibility_win_h__
#define __view_native_view_accessibility_win_h__

#include <atlbase.h>
#include <atlcom.h>

#include "base/memory/scoped_ptr.h"

#include "uibase/accessibility/accessible_view_state.h"

#include "uiview/controls/native/native_view_host.h"
#include "uiview/view.h"

////////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE NativeViewAccessibilityWin
    : public CComObjectRootEx<CComMultiThreadModel>,
    public IDispatchImpl<IAccessible, &IID_IAccessible, &LIBID_Accessibility>
{
public:
    BEGIN_COM_MAP(NativeViewAccessibilityWin)
        COM_INTERFACE_ENTRY2(IDispatch, IAccessible)
        COM_INTERFACE_ENTRY(IAccessible)
    END_COM_MAP()

    static scoped_refptr<NativeViewAccessibilityWin> Create(view::View* view);

    virtual ~NativeViewAccessibilityWin();

    void set_view(view::View* view) { view_ = view; }

    STDMETHODIMP accHitTest(LONG x_left, LONG y_top, VARIANT* child);

    STDMETHODIMP accDoDefaultAction(VARIANT var_id);

    STDMETHODIMP accLocation(LONG* x_left,
        LONG* y_top,
        LONG* width,
        LONG* height,
        VARIANT var_id);

    STDMETHODIMP accNavigate(LONG nav_dir, VARIANT start, VARIANT* end);

    STDMETHODIMP get_accChild(VARIANT var_child, IDispatch** disp_child);

    STDMETHODIMP get_accChildCount(LONG* child_count);

    STDMETHODIMP get_accDefaultAction(VARIANT var_id, BSTR* default_action);

    STDMETHODIMP get_accDescription(VARIANT var_id, BSTR* desc);

    STDMETHODIMP get_accFocus(VARIANT* focus_child);

    STDMETHODIMP get_accKeyboardShortcut(VARIANT var_id, BSTR* access_key);

    STDMETHODIMP get_accName(VARIANT var_id, BSTR* name);

    STDMETHODIMP get_accParent(IDispatch** disp_parent);

    STDMETHODIMP get_accRole(VARIANT var_id, VARIANT* role);

    STDMETHODIMP get_accState(VARIANT var_id, VARIANT* state);

    STDMETHODIMP get_accValue(VARIANT var_id, BSTR* value);

    STDMETHODIMP get_accSelection(VARIANT* selected);
    STDMETHODIMP accSelect(LONG flags_sel, VARIANT var_id);

    STDMETHODIMP get_accHelp(VARIANT var_id, BSTR* help);
    STDMETHODIMP get_accHelpTopic(BSTR* help_file,
        VARIANT var_id, LONG* topic_id);

    STDMETHODIMP put_accName(VARIANT var_id, BSTR put_name);
    STDMETHODIMP put_accValue(VARIANT var_id, BSTR put_val);

    static int32 MSAAEvent(ui::AccessibilityTypes::Event event);

    static int32 MSAARole(ui::AccessibilityTypes::Role role);

    static int32 MSAAState(ui::AccessibilityTypes::State state);

private:
    NativeViewAccessibilityWin();

    bool IsNavDirNext(int nav_dir) const;

    bool IsValidNav(int nav_dir,
        int start_id,
        int lower_bound,
        int upper_bound) const;

    bool IsValidId(const VARIANT& child) const;

    void SetState(VARIANT* msaa_state, view::View* view);

    template<class Base> friend class CComObject;

    view::View* view_;

    DISALLOW_COPY_AND_ASSIGN(NativeViewAccessibilityWin);
};

#endif //__view_native_view_accessibility_win_h__
