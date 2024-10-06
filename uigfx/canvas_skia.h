#ifndef __ui_gfx_canvas_skia_h__
#define __ui_gfx_canvas_skia_h__

#include "canvas.h"
#include "canvas_paint_win.h"

namespace gfx
{
    class CanvasSkia : public skia::PlatformCanvas, public Canvas
    {
    public:
        enum TruncateFadeMode
        {
            TruncateFadeTail,
            TruncateFadeHead,
            TruncateFadeHeadAndTail,
        };

        CanvasSkia();

        CanvasSkia(int width, int height, bool is_opaque);

        virtual ~CanvasSkia();

        static void SizeStringInt(const string16& text,
            const Font& font,
            int* width, int* height,
            int flags);

        static int DefaultCanvasTextAlignment();

        void DrawStringWithHalo(const string16& text,
            const Font& font,
            const SkColor& text_color,
            const SkColor& halo_color,
            int x, int y, int w, int h,
            int flags);

        SkBitmap ExtractBitmap() const;

        virtual void Save();
        virtual void SaveLayerAlpha(uint8 alpha);
        virtual void SaveLayerAlpha(uint8 alpha, const Rect& layer_bounds);
        virtual void Restore();
        virtual bool ClipRectInt(int x, int y, int w, int h);
        virtual void TranslateInt(int x, int y);
        virtual void ScaleInt(int x, int y);
        virtual void FillRectInt(const SkColor& color, int x, int y, int w, int h);
        virtual void FillRectInt(const SkColor& color, int x, int y, int w, int h,
            SkXfermode::Mode mode);
        virtual void FillRectInt(const Brush* brush, int x, int y, int w, int h);
        virtual void DrawRectInt(const SkColor& color, int x, int y, int w, int h);
        virtual void DrawRectInt(const SkColor& color, int x, int y, int w, int h,
            SkXfermode::Mode mode);
        virtual void DrawRectInt(int x, int y, int w, int h, const SkPaint& paint);
        virtual void DrawLineInt(const SkColor& color, int x1, int y1, int x2, int y2);
        virtual void DrawBitmapInt(const SkBitmap& bitmap, int x, int y);
        virtual void DrawBitmapInt(const SkBitmap& bitmap, int x, int y,
            const SkPaint& paint);
        virtual void DrawBitmapInt(const SkBitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h,
            bool filter);
        virtual void DrawBitmapInt(const SkBitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h,
            bool filter,
            const SkPaint& paint);
        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            int x, int y, int w, int h);
        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            const Rect& display_rect);
        virtual void DrawStringInt(const string16& text,
            const Font& font,
            const SkColor& color,
            int x, int y, int w, int h,
            int flags);
        // Draws the given string with the beginning and/or the end using a fade
        // gradient. When truncating the head
        // |desired_characters_to_truncate_from_head| specifies the maximum number of
        // characters that can be truncated.
        virtual void DrawFadeTruncatingString(
            const string16& text,
            TruncateFadeMode truncate_mode,
            size_t desired_characters_to_truncate_from_head,
            const gfx::Font& font,
            const SkColor& color,
            const gfx::Rect& display_rect);
        virtual void DrawFocusRect(int x, int y, int width, int height);
        virtual void TileImageInt(const SkBitmap& bitmap, int x, int y, int w, int h);
        virtual void TileImageInt(const SkBitmap& bitmap,
            int src_x, int src_y, int dest_x, int dest_y, int w, int h);
        virtual HDC BeginPlatformPaint();
        virtual void EndPlatformPaint();
        virtual void ConcatTransform(const Transform& transform);
        virtual CanvasSkia* AsCanvasSkia();
        virtual const CanvasSkia* AsCanvasSkia() const;

    private:
        bool IntersectsClipRectInt(int x, int y, int w, int h);

        void DrawStringInt(const string16& text,
            HFONT font,
            const SkColor& color,
            int x, int y, int w, int h,
            int flags);

        DISALLOW_COPY_AND_ASSIGN(CanvasSkia);
    };

    typedef CanvasPaintT<CanvasSkia> CanvasSkiaPaint;

} //namespace gfx

#endif //__ui_gfx_canvas_skia_h__
