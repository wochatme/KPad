#ifndef __view_drop_helper_h__
#define __view_drop_helper_h__

#include <objidl.h>

#include "base/basic_types.h"

namespace gfx
{
    class Point;
}

namespace view
{
    class View;

    class DropHelper
    {
    public:
        explicit DropHelper(View* root_view);
        ~DropHelper();

        View* target_view() const { return target_view_; }

        View* root_view() const { return root_view_; }

        void ResetTargetViewIfEquals(View* view);

        DWORD OnDragOver(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);

        void OnDragLeave();

        DWORD OnDrop(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);

        View* CalculateTargetView(const gfx::Point& root_view_location,
            IDataObject* data_object);

    private:
        View* CalculateTargetViewImpl(const gfx::Point& root_view_location,
            IDataObject* data_object,
            View** deepest_view);

        void NotifyDragEntered(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);
        DWORD NotifyDragOver(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);
        void NotifyDragLeave();

        View* root_view_;

        View* target_view_;

        View* deepest_view_;

        DISALLOW_COPY_AND_ASSIGN(DropHelper);
    };

} //namespace view

#endif //__view_drop_helper_h__
