#ifndef __ui_base_view_prop_h__
#define __ui_base_view_prop_h__

#include "base/basic_types.h"
#include "base/memory/ref_counted.h"

namespace ui
{
    class ViewProp
    {
    public:
        ViewProp(HWND view, const char* key, void* data);
        ~ViewProp();

        static void* GetValue(HWND view, const char* key);

        const char* Key() const;

    private:
        class Data;

        scoped_refptr<Data> data_;

        DISALLOW_COPY_AND_ASSIGN(ViewProp);
    };

} //namespace ui

#endif //__ui_base_view_prop_h__

