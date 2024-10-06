#ifndef __ui_base_accessible_view_state_h__
#define __ui_base_accessible_view_state_h__


#include "base/string16.h"
#include "accessibility_types.h"

namespace ui
{
    ////////////////////////////////////////////////////////////////////////////////
    //
    // AccessibleViewState
    //
    //
    ////////////////////////////////////////////////////////////////////////////////
    struct AccessibleViewState
    {
    public:
        AccessibleViewState();
        ~AccessibleViewState();

        AccessibilityTypes::Role role;

        AccessibilityTypes::State state;

        string16 name;

        string16 value;

        string16 default_action;

        string16 keyboard_shortcut;

        int selection_start;
        int selection_end;

        int index;
        int count;
    };

} //namespace ui

#endif //__ui_base_accessible_view_state_h__

