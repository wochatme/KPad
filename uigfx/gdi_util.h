#ifndef __ui_gfx_gdi_util_h__
#define __ui_gfx_gdi_util_h__

#include <vector>
#include <windows.h>

#include "rect.h"

namespace gfx
{
    void CreateBitmapHeader(int width, int height, BITMAPINFOHEADER* hdr);

    void CreateBitmapHeaderWithColorDepth(int width, int height,
        int color_depth, BITMAPINFOHEADER* hdr);

    void CreateBitmapV4Header(int width, int height, BITMAPV4HEADER* hdr);

    void CreateMonochromeBitmapHeader(int width, int height,
        BITMAPINFOHEADER* hdr);

    void SubtractRectanglesFromRegion(HRGN hrgn,
        const std::vector<gfx::Rect>& cutouts);

} //namespace gfx

#endif //__ui_gfx_gdi_util_h__