#ifndef __view_event_h__
#define __view_event_h__

#pragma once

#include "base/base_time.h"
#include "uigfx/point.h"
#include "uibase/events.h"
#include "uibase/keycodes/keyboard_codes_win.h"

namespace ui
{
    class OSExchangeData;
}

namespace view
{

    class View;

    namespace internal
    {
        class NativeWidgetView;
        class RootView;
    }

    bool IsClientMouseEvent(const MSG& native_event);
    bool IsNonClientMouseEvent(const MSG& native_event);

    class Event
    {
    public:
        const MSG& native_event() const { return native_event_; }
        ui::EventType type() const { return type_; }
        const base::Time& time_stamp() const { return time_stamp_; }
        int flags() const { return flags_; }
        void set_flags(int flags) { flags_ = flags; }

        bool IsShiftDown() const
        {
            return (flags_ & ui::EF_SHIFT_DOWN) != 0;
        }

        bool IsControlDown() const
        {
            return (flags_ & ui::EF_CONTROL_DOWN) != 0;
        }

        bool IsCapsLockDown() const
        {
            return (flags_ & ui::EF_CAPS_LOCK_DOWN) != 0;
        }

        bool IsAltDown() const
        {
            return (flags_ & ui::EF_ALT_DOWN) != 0;
        }

        bool IsMouseEvent() const
        {
            return type_ == ui::ET_MOUSE_PRESSED ||
                type_ == ui::ET_MOUSE_DRAGGED ||
                type_ == ui::ET_MOUSE_RELEASED ||
                type_ == ui::ET_MOUSE_MOVED ||
                type_ == ui::ET_MOUSE_ENTERED ||
                type_ == ui::ET_MOUSE_EXITED ||
                type_ == ui::ET_MOUSEWHEEL;
        }

        int GetWindowsFlags() const;

    protected:
        Event(ui::EventType type, int flags);
        Event(MSG native_event, ui::EventType type, int flags);

        Event(const Event& model)
            : native_event_(model.native_event()),
            type_(model.type()),
            time_stamp_(model.time_stamp()),
            flags_(model.flags()) {}

        void set_type(ui::EventType type) { type_ = type; }

    private:
        void operator=(const Event&);

        void Init();
        void InitWithNativeEvent(MSG native_event);

        MSG native_event_;
        ui::EventType type_;
        base::Time time_stamp_;
        int flags_;
    };

    class LocatedEvent : public Event
    {
    public:
        int x() const { return location_.x(); }
        int y() const { return location_.y(); }
        const gfx::Point& location() const { return location_; }

    protected:
        explicit LocatedEvent(MSG native_event);

        LocatedEvent(ui::EventType type, const gfx::Point& location, int flags);

        LocatedEvent(const LocatedEvent& model, View* source, View* target);

        LocatedEvent(const LocatedEvent& model, View* root);

        gfx::Point location_;
    };

    class MouseEvent : public LocatedEvent
    {
    public:
        explicit MouseEvent(MSG native_event);

        // Create a new MouseEvent which is identical to the provided model.
        // If source / target views are provided, the model location will be converted
        // from |source| coordinate system to |target| coordinate system.
        MouseEvent(const MouseEvent& model, View* source, View* target);

        // TODO(msw): Kill this legacy constructor when we update uses.
        // Create a new mouse event
        MouseEvent(ui::EventType type, int x, int y, int flags)
            : LocatedEvent(type, gfx::Point(x, y), flags) {}

        bool IsOnlyLeftMouseButton() const
        {
            return (flags() & ui::EF_LEFT_BUTTON_DOWN) &&
                !(flags() & (ui::EF_MIDDLE_BUTTON_DOWN | ui::EF_RIGHT_BUTTON_DOWN));
        }

        bool IsLeftMouseButton() const
        {
            return (flags() & ui::EF_LEFT_BUTTON_DOWN) != 0;
        }

        bool IsOnlyMiddleMouseButton() const
        {
            return (flags() & ui::EF_MIDDLE_BUTTON_DOWN) &&
                !(flags() & (ui::EF_LEFT_BUTTON_DOWN | ui::EF_RIGHT_BUTTON_DOWN));
        }

        bool IsMiddleMouseButton() const
        {
            return (flags() & ui::EF_MIDDLE_BUTTON_DOWN) != 0;
        }

        bool IsOnlyRightMouseButton() const
        {
            return (flags() & ui::EF_RIGHT_BUTTON_DOWN) &&
                !(flags() & (ui::EF_LEFT_BUTTON_DOWN | ui::EF_MIDDLE_BUTTON_DOWN));
        }

        bool IsRightMouseButton() const
        {
            return (flags() & ui::EF_RIGHT_BUTTON_DOWN) != 0;
        }

    protected:
        MouseEvent(const MouseEvent& model, View* root)
            : LocatedEvent(model, root) {}

    private:
        friend class internal::NativeWidgetView;
        friend class internal::RootView;

        DISALLOW_COPY_AND_ASSIGN(MouseEvent);
    };

    class KeyEvent : public Event
    {
    public:
        explicit KeyEvent(MSG native_event);

        // Creates a new KeyEvent synthetically (i.e. not in response to an input
        // event from the host environment). This is typically only used in testing as
        // some metadata obtainable from the underlying native event is not present.
        // It's also used by input methods to fabricate keyboard events.
        KeyEvent(ui::EventType type, ui::KeyboardCode key_code, int event_flags);

        ui::KeyboardCode key_code() const { return key_code_; }

        // These setters allow an I18N virtual keyboard to fabricate a keyboard event
        // which does not have a corresponding ui::KeyboardCode (example: U+00E1 Latin
        // small letter A with acute, U+0410 Cyrillic capital letter A.)
        // GetCharacter() and GetUnmodifiedCharacter() return the character.
        void set_character(uint16 character) { character_ = character; }
        void set_unmodified_character(uint16 unmodified_character)
        {
            unmodified_character_ = unmodified_character;
        }

        // Gets the character generated by this key event. It only supports Unicode
        // BMP characters.
        uint16 GetCharacter() const;

        // Gets the character generated by this key event ignoring concurrently-held
        // modifiers (except shift).
        uint16 GetUnmodifiedCharacter() const;

    private:
        // A helper function to get the character generated by a key event in a
        // platform independent way. It supports control characters as well.
        // It assumes a US keyboard layout is used, so it may only be used when there
        // is no native event or no better way to get the character.
        // For example, if a virtual keyboard implementation can only generate key
        // events with key_code and flags information, then there is no way for us to
        // determine the actual character that should be generate by the key. Because
        // a key_code only represents a physical key on the keyboard, it has nothing
        // to do with the actual character printed on that key. In such case, the only
        // thing we can do is to assume that we are using a US keyboard and get the
        // character according to US keyboard layout definition.
        // If a virtual keyboard implementation wants to support other keyboard
        // layouts, that may generate different text for a certain key than on a US
        // keyboard, a special native event object should be introduced to carry extra
        // information to help determine the correct character.
        // Take XKeyEvent as an example, it contains not only keycode and modifier
        // flags but also group and other extra XKB information to help determine the
        // correct character. That's why we can use XLookupString() function to get
        // the correct text generated by a X key event (See how is GetCharacter()
        // implemented in event_x.cc).
        // TODO(suzhe): define a native event object for virtual keyboard. We may need
        // to take the actual feature requirement into account.
        static uint16 GetCharacterFromKeyCode(ui::KeyboardCode key_code, int flags);

        ui::KeyboardCode key_code_;

        uint16 character_;
        uint16 unmodified_character_;

        DISALLOW_COPY_AND_ASSIGN(KeyEvent);
    };

    class MouseWheelEvent : public MouseEvent
    {
    public:
        static const int kWheelDelta;

        explicit MouseWheelEvent(MSG native_event);

        int offset() const { return offset_; }

    private:
        friend class internal::NativeWidgetView;
        friend class internal::RootView;

        MouseWheelEvent(const MouseWheelEvent& model, View* root)
            : MouseEvent(model, root), offset_(model.offset_) {}

        int offset_;

        DISALLOW_COPY_AND_ASSIGN(MouseWheelEvent);
    };

    class DropTargetEvent : public LocatedEvent
    {
    public:
        DropTargetEvent(const ui::OSExchangeData& data,
            int x,
            int y,
            int source_operations)
            : LocatedEvent(ui::ET_DROP_TARGET_EVENT, gfx::Point(x, y), 0),
            data_(data),
            source_operations_(source_operations) {}

        const ui::OSExchangeData& data() const { return data_; }
        int source_operations() const { return source_operations_; }

    private:
        const ui::OSExchangeData& data_;

        int source_operations_;

        DISALLOW_COPY_AND_ASSIGN(DropTargetEvent);
    };

} //namespace view

#endif //__view_event_h__