#ifndef __ui_base_drop_target_h__
#define __ui_base_drop_target_h__

#include <objidl.h>

#include "base/memory/ref_counted.h"

struct IDropTargetHelper;

namespace ui
{
    class DropTarget : public IDropTarget
    {
    public:
        explicit DropTarget(HWND hwnd);
        virtual ~DropTarget();

        bool suspended() const { return suspended_; }
        void set_suspended(bool suspended) { suspended_ = suspended; }

        HRESULT __stdcall DragEnter(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);
        HRESULT __stdcall DragOver(DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);
        HRESULT __stdcall DragLeave();
        HRESULT __stdcall Drop(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);

        HRESULT __stdcall QueryInterface(const IID& iid, void** object);
        ULONG __stdcall AddRef();
        ULONG __stdcall Release();

    protected:
        HWND GetHWND() { return hwnd_; }

        virtual DWORD OnDragEnter(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD effect);

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
        static IDropTargetHelper* DropHelper();

        scoped_refptr<IDataObject> current_data_object_;

        static IDropTargetHelper* cached_drop_target_helper_;

        HWND hwnd_;

        bool suspended_;

        LONG ref_count_;

        DISALLOW_COPY_AND_ASSIGN(DropTarget);
    };

} //namespace ui

#endif //__ui_base_drop_target_h__
