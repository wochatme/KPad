#ifndef __ui_base_accessibility_types_h__
#define __ui_base_accessibility_types_h__

#include "base/basic_types.h"

namespace ui
{
    ////////////////////////////////////////////////////////////////////////////////
    //
    // AccessibilityTypes
    //
    //
    ////////////////////////////////////////////////////////////////////////////////
    class AccessibilityTypes
    {
    public:
        typedef uint32 State;
        enum StateFlag
        {
            STATE_CHECKED = 1 << 0,
            STATE_COLLAPSED = 1 << 1,
            STATE_DEFAULT = 1 << 2,
            STATE_EXPANDED = 1 << 3,
            STATE_HASPOPUP = 1 << 4,
            STATE_HOTTRACKED = 1 << 5,
            STATE_INVISIBLE = 1 << 6,
            STATE_LINKED = 1 << 7,
            STATE_OFFSCREEN = 1 << 8,
            STATE_PRESSED = 1 << 9,
            STATE_PROTECTED = 1 << 10,
            STATE_READONLY = 1 << 11,
            STATE_SELECTED = 1 << 12,
            STATE_FOCUSED = 1 << 13,
            STATE_UNAVAILABLE = 1 << 14
        };

        enum Role
        {
            ROLE_ALERT,
            ROLE_APPLICATION,
            ROLE_BUTTONDROPDOWN,
            ROLE_BUTTONMENU,
            ROLE_CHECKBUTTON,
            ROLE_CLIENT,
            ROLE_COMBOBOX,
            ROLE_DIALOG,
            ROLE_GRAPHIC,
            ROLE_GROUPING,
            ROLE_LINK,
            ROLE_LOCATION_BAR,
            ROLE_MENUBAR,
            ROLE_MENUITEM,
            ROLE_MENUPOPUP,
            ROLE_OUTLINE,
            ROLE_OUTLINEITEM,
            ROLE_PAGETAB,
            ROLE_PAGETABLIST,
            ROLE_PANE,
            ROLE_PROGRESSBAR,
            ROLE_PUSHBUTTON,
            ROLE_RADIOBUTTON,
            ROLE_SCROLLBAR,
            ROLE_SEPARATOR,
            ROLE_STATICTEXT,
            ROLE_TEXT,
            ROLE_TITLEBAR,
            ROLE_TOOLBAR,
            ROLE_WINDOW
        };

        enum Event
        {
            EVENT_ALERT,
            EVENT_FOCUS,
            EVENT_MENUSTART,
            EVENT_MENUEND,
            EVENT_MENUPOPUPSTART,
            EVENT_MENUPOPUPEND,
            EVENT_NAME_CHANGED,
            EVENT_TEXT_CHANGED,
            EVENT_SELECTION_CHANGED,
            EVENT_VALUE_CHANGED
        };

    private:
        AccessibilityTypes() {}
        ~AccessibilityTypes() {}
    };

} //namespace ui

#endif //__ui_base_accessibility_types_h__