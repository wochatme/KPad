#ifndef __ui_gfx_brush_h__
#define __ui_gfx_brush_h__

#include "base/basic_types.h"

namespace gfx
{
    class Brush
    {
    public:
        Brush() {}
        virtual ~Brush() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(Brush);
    };

} //namespace gfx

#endif //__ui_gfx_brush_h__