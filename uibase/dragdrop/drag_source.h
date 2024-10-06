#ifndef __ui_base_drag_source_h__
#define __ui_base_drag_source_h__

#include <objidl.h>

#include "base/memory/ref_counted.h"

namespace ui
{
    class DragSource : public IDropSource,
        public base::RefCountedThreadSafe<DragSource>
    {
    public:
        DragSource();
        virtual ~DragSource() {}

        void CancelDrag()
        {
            cancel_drag_ = true;
        }

        HRESULT __stdcall QueryContinueDrag(BOOL escape_pressed, DWORD key_state);
        HRESULT __stdcall GiveFeedback(DWORD effect);

        HRESULT __stdcall QueryInterface(const IID& iid, void** object);
        ULONG __stdcall AddRef();
        ULONG __stdcall Release();

    protected:
        virtual void OnDragSourceCancel() {}
        virtual void OnDragSourceDrop() {}
        virtual void OnDragSourceMove() {}

    private:
        bool cancel_drag_;

        DISALLOW_COPY_AND_ASSIGN(DragSource);
    };

} //namespace ui

#endif //__ui_base_drag_source_h__
