#ifndef __ui_gfx_color_utils_h__
#define __ui_gfx_color_utils_h__

#pragma once

#include "SkColor.h"

class SkBitmap;

namespace gfx
{
    struct HSL
    {
        double h;
        double s;
        double l;
    };

    unsigned char GetLuminanceForColor(SkColor color);

    // http://www.w3.org/TR/WCAG20/#relativeluminancedef 
    double RelativeLuminance(SkColor color);

    void SkColorToHSL(SkColor c, HSL* hsl);
    SkColor HSLToSkColor(const HSL& hsl, SkAlpha alpha);

    // HSL-Shift an SkColor. The shift values are in the range of 0-1, with the
    // option to specify -1 for 'no change'. The shift values are defined as:
    // hsl_shift[0] (hue): The absolute hue value - 0 and 1 map
    //    to 0 and 360 on the hue color wheel (red).
    // hsl_shift[1] (saturation): A saturation shift, with the
    //    following key values:
    //    0 = remove all color.
    //    0.5 = leave unchanged.
    //    1 = fully saturate the image.
    // hsl_shift[2] (lightness): A lightness shift, with the
    //    following key values:
    //    0 = remove all lightness (make all pixels black).
    //    0.5 = leave unchanged.
    //    1 = full lightness (make all pixels white).
    SkColor HSLShift(SkColor color, const HSL& shift);

    bool IsColorCloseToTransparent(SkAlpha alpha);

    bool IsColorCloseToGrey(int r, int g, int b);

    SkColor GetAverageColorOfFavicon(SkBitmap* bitmap, SkAlpha alpha);

    void BuildLumaHistogram(SkBitmap* bitmap, int histogram[256]);

    SkColor AlphaBlend(SkColor foreground, SkColor background, SkAlpha alpha);

    SkColor GetReadableColor(SkColor foreground, SkColor background);

    SkColor GetSysSkColor(int which);

} //namespace gfx

#endif //__ui_gfx_color_utils_h__