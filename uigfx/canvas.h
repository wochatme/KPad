#ifndef __ui_gfx_canvas_h__
#define __ui_gfx_canvas_h__

#include <windows.h>

#include "base/basic_types.h"
#include "base/string16.h"

// TODO(beng): remove these includes when we no longer depend on SkTypes.
#include "SkColor.h"
#include "SkXfermode.h"

namespace gfx
{
    class Brush;
    class CanvasSkia;
    class Font;
    class Point;
    class Rect;
    class Transform;

    class Canvas
    {
    public:
        enum
        {
            TEXT_ALIGN_LEFT = 1,
            TEXT_ALIGN_CENTER = 2,
            TEXT_ALIGN_RIGHT = 4,
            TEXT_VALIGN_TOP = 8,
            TEXT_VALIGN_MIDDLE = 16,
            TEXT_VALIGN_BOTTOM = 32,

            MULTI_LINE = 64,

            SHOW_PREFIX = 128,
            HIDE_PREFIX = 256,

            NO_ELLIPSIS = 512,

            CHARACTER_BREAK = 1024,

            FORCE_RTL_DIRECTIONALITY = 2048,

            // Similar to FORCE_RTL_DIRECTIONALITY, but left-to-right.
            // See FORCE_RTL_DIRECTIONALITY for details.
            FORCE_LTR_DIRECTIONALITY = 4096,
        };

        virtual ~Canvas() {}

        static Canvas* CreateCanvas();

        static Canvas* CreateCanvas(int width, int height, bool is_opaque);

        virtual void Save() = 0;

        virtual void SaveLayerAlpha(uint8 alpha) = 0;
        virtual void SaveLayerAlpha(uint8 alpha, const Rect& layer_bounds) = 0;

        virtual void Restore() = 0;

        virtual bool ClipRectInt(int x, int y, int w, int h) = 0;

        virtual void TranslateInt(int x, int y) = 0;

        virtual void ScaleInt(int x, int y) = 0;

        virtual void FillRectInt(const SkColor& color,
            int x, int y, int w, int h) = 0;

        virtual void FillRectInt(const SkColor& color,
            int x, int y, int w, int h, SkXfermode::Mode mode) = 0;

        virtual void FillRectInt(const Brush* brush,
            int x, int y, int w, int h) = 0;

        virtual void DrawRectInt(const SkColor& color,
            int x, int y, int w, int h) = 0;

        virtual void DrawRectInt(const SkColor& color,
            int x, int y, int w, int h,
            SkXfermode::Mode mode) = 0;

        virtual void DrawRectInt(int x, int y, int w, int h,
            const SkPaint& paint) = 0;

        virtual void DrawLineInt(const SkColor& color,
            int x1, int y1, int x2, int y2) = 0;

        virtual void DrawBitmapInt(const SkBitmap& bitmap, int x, int y) = 0;

        virtual void DrawBitmapInt(const SkBitmap& bitmap,
            int x, int y, const SkPaint& paint) = 0;

        virtual void DrawBitmapInt(const SkBitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h,
            bool filter) = 0;
        virtual void DrawBitmapInt(const SkBitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h,
            bool filter, const SkPaint& paint) = 0;

        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            int x, int y, int w, int h) = 0;
        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            const Rect& display_rect) = 0;

        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            int x, int y, int w, int h,
            int flags) = 0;

        virtual void DrawFocusRect(int x, int y, int width, int height) = 0;

        virtual void TileImageInt(const SkBitmap& bitmap,
            int x, int y, int w, int h) = 0;
        virtual void TileImageInt(const SkBitmap& bitmap,
            int src_x, int src_y,
            int dest_x, int dest_y, int w, int h) = 0;

        virtual HDC BeginPlatformPaint() = 0;

        virtual void EndPlatformPaint() = 0;

        virtual void ConcatTransform(const Transform& transform) = 0;

        virtual CanvasSkia* AsCanvasSkia();
        virtual const CanvasSkia* AsCanvasSkia() const;
    };

    class CanvasPaint
    {
    public:
        virtual ~CanvasPaint() {}

        static CanvasPaint* CreateCanvasPaint(HWND view);

        virtual bool IsValid() const = 0;

        virtual Rect GetInvalidRect() const = 0;

        virtual Canvas* AsCanvas() = 0;
    };

} //namespace gfx

#endif //__ui_gfx_canvas_h__
