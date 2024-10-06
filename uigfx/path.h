#ifndef __ui_gfx_path_h__
#define __ui_gfx_path_h__

#include "base/basic_types.h"

#include "SkPath.h"

#include "point.h"

namespace gfx
{
    class Path : public SkPath
    {
    public:
        Path();
        Path(const Point* points, size_t count);
        ~Path();

        HRGN CreateNativeRegion() const;

        static HRGN IntersectRegions(HRGN r1, HRGN r2);

        static HRGN CombineRegions(HRGN r1, HRGN r2);

        static HRGN SubtractRegion(HRGN r1, HRGN r2);

    private:
        DISALLOW_COPY_AND_ASSIGN(Path);
    };

} //namespace gfx

#endif //__ui_gfx_path_h__