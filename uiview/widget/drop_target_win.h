#ifndef __view_drop_target_win_h__
#define __view_drop_target_win_h__

#include "uibase/dragdrop/drop_target.h"

#include "drop_helper.h"

namespace view
{
    class View;
    namespace internal
    {
        class RootView;
    }

    class DropTargetWin : public ui::DropTarget
    {
    public:
        explicit DropTargetWin(internal::RootView* root_view);
        virtual ~DropTargetWin();

        void ResetTargetViewIfEquals(View* view);

    protected:
        virtual DWORD OnDragOver(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);

        virtual void OnDragLeave(IDataObject* data_object);

        virtual DWORD OnDrop(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);

    private:
        view::DropHelper helper_;

        DISALLOW_COPY_AND_ASSIGN(DropTargetWin);
    };

} //namespace view

#endif //__view_drop_target_win_h__

