#ifndef __ui_gfx_skia_util_h__
#define __ui_gfx_skia_util_h__

#include <string>

#include "SkColor.h"
#include "SkRect.h"

class SkBitmap;
class SkShader;

namespace gfx
{
    class Rect;

    SkRect RectToSkRect(const gfx::Rect& rect);
    gfx::Rect SkRectToRect(const SkRect& rect);

    //     SkSafeUnref(paint.setShader(gfx::CreateGradientShader(0, 10, red, blue)));
    //
    SkShader* CreateGradientShader(int start_point, int end_point,
        SkColor start_color, SkColor end_color);

    bool BitmapsAreEqual(const SkBitmap& bitmap1, const SkBitmap& bitmap2);

    // Strip the accelerator char (typically '&') from a menu string.  A
    // double accelerator char ('&&') will be converted to a single char.
    std::string RemoveAcceleratorChar(const std::string& s, char accelerator_char);

} //namespace gfx

#endif //__ui_gfx_skia_util_h__

