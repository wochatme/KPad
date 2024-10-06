#if 0
#include "canvas_skia.h"

#include <limits>

#include "base/i18n/rtl.h"
#include "base/logging.h"

#include "SkGradientShader.h"

#include "brush.h"
#include "font.h"
#include "rect.h"
#include "transform.h"

namespace
{
    class SkiaShader : public gfx::Brush
    {
    public:
        explicit SkiaShader(SkShader* shader) : shader_(shader) {}
        virtual ~SkiaShader()
        {
            shader_->unref();
        }

        SkShader* shader() const { return shader_; }

    private:
        SkShader* shader_;

        DISALLOW_COPY_AND_ASSIGN(SkiaShader);
    };

}

namespace gfx
{

    // static
    int CanvasSkia::DefaultCanvasTextAlignment()
    {
        if (!base::i18n::IsRTL())
        {
            return Canvas::TEXT_ALIGN_LEFT;
        }
        return Canvas::TEXT_ALIGN_RIGHT;
    }

    SkBitmap CanvasSkia::ExtractBitmap() const
    {
        const SkBitmap& device_bitmap = getDevice()->accessBitmap(false);

        SkBitmap result;
        device_bitmap.copyTo(&result, SkBitmap::kARGB_8888_Config);
        return result;
    }

    void CanvasSkia::Save()
    {
        save();
    }

    void CanvasSkia::SaveLayerAlpha(uint8 alpha)
    {
        saveLayerAlpha(NULL, alpha);
    }

    void CanvasSkia::SaveLayerAlpha(uint8 alpha, const Rect& layer_bounds)
    {
        SkRect bounds;
        bounds.set(SkIntToScalar(layer_bounds.x()),
            SkIntToScalar(layer_bounds.y()),
            SkIntToScalar(layer_bounds.right()),
            SkIntToScalar(layer_bounds.bottom()));
        saveLayerAlpha(&bounds, alpha);
    }

    void CanvasSkia::Restore()
    {
        restore();
    }

    bool CanvasSkia::ClipRectInt(int x, int y, int w, int h)
    {
        SkRect new_clip;
        new_clip.set(SkIntToScalar(x), SkIntToScalar(y),
            SkIntToScalar(x + w), SkIntToScalar(y + h));
        return clipRect(new_clip);
    }

    void CanvasSkia::TranslateInt(int x, int y)
    {
        translate(SkIntToScalar(x), SkIntToScalar(y));
    }

    void CanvasSkia::ScaleInt(int x, int y)
    {
        scale(SkIntToScalar(x), SkIntToScalar(y));
    }

    void CanvasSkia::FillRectInt(const SkColor& color, int x, int y, int w, int h)
    {
        FillRectInt(color, x, y, w, h, SkXfermode::kSrcOver_Mode);
    }

    void CanvasSkia::FillRectInt(const SkColor& color, int x, int y, int w, int h,
        SkXfermode::Mode mode)
    {
        SkPaint paint;
        paint.setColor(color);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setXfermodeMode(mode);
        DrawRectInt(x, y, w, h, paint);
    }

    void CanvasSkia::FillRectInt(const Brush* brush, int x, int y, int w, int h)
    {
        const SkiaShader* shader = static_cast<const SkiaShader*>(brush);
        SkPaint paint;
        paint.setShader(shader->shader());
        DrawRectInt(x, y, w, h, paint);
    }

    void CanvasSkia::DrawRectInt(const SkColor& color, int x, int y, int w, int h)
    {
        DrawRectInt(color, x, y, w, h, SkXfermode::kSrcOver_Mode);
    }

    void CanvasSkia::DrawRectInt(const SkColor& color,
        int x, int y, int w, int h, SkXfermode::Mode mode)
    {
        SkPaint paint;
        paint.setColor(color);
        paint.setStyle(SkPaint::kStroke_Style);

        paint.setStrokeWidth(SkIntToScalar(0));
        paint.setXfermodeMode(mode);

        DrawRectInt(x, y, w, h, paint);
    }

    void CanvasSkia::DrawRectInt(int x, int y, int w, int h, const SkPaint& paint)
    {
        SkIRect rc = { x, y, x + w, y + h };
        drawIRect(rc, paint);
    }

    void CanvasSkia::DrawLineInt(const SkColor& color,
        int x1, int y1, int x2, int y2)
    {
        SkPaint paint;
        paint.setColor(color);
        paint.setStrokeWidth(SkIntToScalar(1));
        drawLine(SkIntToScalar(x1), SkIntToScalar(y1), SkIntToScalar(x2),
            SkIntToScalar(y2), paint);
    }

    void CanvasSkia::DrawFocusRect(int x, int y, int width, int height)
    {
        static SkBitmap* dots = NULL;
        if (!dots)
        {
            int col_pixels = 32;
            int row_pixels = 32;

            dots = new SkBitmap;
            dots->setConfig(SkBitmap::kARGB_8888_Config, col_pixels, row_pixels);
            dots->allocPixels();
            dots->eraseARGB(0, 0, 0, 0);

            uint32_t* dot = dots->getAddr32(0, 0);
            for (int i = 0; i < row_pixels; i++)
            {
                for (int u = 0; u < col_pixels; u++)
                {
                    if ((u % 2 + i % 2) % 2 != 0)
                    {
                        dot[i * row_pixels + u] = SK_ColorGRAY;
                    }
                }
            }
        }

        SkShader* shader = SkShader::CreateBitmapShader(
            *dots, SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode);
        SkPaint paint;
        paint.setShader(shader);
        shader->unref();

        DrawRectInt(x, y, width, 1, paint);
        DrawRectInt(x, y + height - 1, width, 1, paint);
        DrawRectInt(x, y, 1, height, paint);
        DrawRectInt(x + width - 1, y, 1, height, paint);
    }

    void CanvasSkia::DrawBitmapInt(const SkBitmap& bitmap, int x, int y)
    {
        drawBitmap(bitmap, SkIntToScalar(x), SkIntToScalar(y));
    }

    void CanvasSkia::DrawBitmapInt(const SkBitmap& bitmap,
        int x, int y, const SkPaint& paint)
    {
        drawBitmap(bitmap, SkIntToScalar(x), SkIntToScalar(y), &paint);
    }

    void CanvasSkia::DrawBitmapInt(const SkBitmap& bitmap,
        int src_x, int src_y, int src_w, int src_h,
        int dest_x, int dest_y, int dest_w, int dest_h,
        bool filter)
    {
        SkPaint p;
        DrawBitmapInt(bitmap, src_x, src_y, src_w, src_h, dest_x, dest_y,
            dest_w, dest_h, filter, p);
    }

    void CanvasSkia::DrawBitmapInt(const SkBitmap& bitmap,
        int src_x, int src_y, int src_w, int src_h,
        int dest_x, int dest_y, int dest_w, int dest_h,
        bool filter,
        const SkPaint& paint)
    {
        DLOG_ASSERT(src_x + src_w < std::numeric_limits<int16_t>::max() &&
            src_y + src_h < std::numeric_limits<int16_t>::max());
        if (src_w <= 0 || src_h <= 0 || dest_w <= 0 || dest_h <= 0)
        {
            NOTREACHED() << "Attempting to draw bitmap to/from an empty rect!";
            return;
        }

        if (!IntersectsClipRectInt(dest_x, dest_y, dest_w, dest_h))
        {
            return;
        }

        SkRect dest_rect = { SkIntToScalar(dest_x),
            SkIntToScalar(dest_y),
            SkIntToScalar(dest_x + dest_w),
            SkIntToScalar(dest_y + dest_h) };

        if (src_w == dest_w && src_h == dest_h)
        {
            SkIRect src_rect = { src_x, src_y, src_x + src_w, src_y + src_h };
            drawBitmapRect(bitmap, &src_rect, dest_rect, &paint);
            return;
        }

        SkShader* shader = SkShader::CreateBitmapShader(bitmap,
            SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode);
        SkMatrix shader_scale;
        shader_scale.setScale(SkFloatToScalar(static_cast<float>(dest_w) / src_w),
            SkFloatToScalar(static_cast<float>(dest_h) / src_h));
        shader_scale.preTranslate(SkIntToScalar(-src_x), SkIntToScalar(-src_y));
        shader_scale.postTranslate(SkIntToScalar(dest_x), SkIntToScalar(dest_y));
        shader->setLocalMatrix(shader_scale);

        SkPaint p(paint);
        p.setFilterBitmap(filter);
        p.setShader(shader);
        shader->unref();

        drawRect(dest_rect, p);
    }

    void CanvasSkia::DrawStringInt(const string16& text,
        const Font& font,
        const SkColor& color,
        int x, int y, int w, int h)
    {
        DrawStringInt(text, font, color, x, y, w, h,
            CanvasSkia::DefaultCanvasTextAlignment());
    }

    void CanvasSkia::DrawStringInt(const string16& text,
        const Font& font,
        const SkColor& color,
        const Rect& display_rect)
    {
        DrawStringInt(text, font, color, display_rect.x(), display_rect.y(),
            display_rect.width(), display_rect.height());
    }

    void CanvasSkia::TileImageInt(const SkBitmap& bitmap,
        int x, int y, int w, int h)
    {
        TileImageInt(bitmap, 0, 0, x, y, w, h);
    }

    void CanvasSkia::TileImageInt(const SkBitmap& bitmap,
        int src_x, int src_y,
        int dest_x, int dest_y, int w, int h)
    {
        if (!IntersectsClipRectInt(dest_x, dest_y, w, h))
        {
            return;
        }

        SkPaint paint;

        SkShader* shader = SkShader::CreateBitmapShader(bitmap,
            SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode);
        paint.setShader(shader);
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

        shader->unref();
        save();
        translate(SkIntToScalar(dest_x - src_x), SkIntToScalar(dest_y - src_y));
        ClipRectInt(src_x, src_y, w, h);
        drawPaint(paint);
        restore();
    }

    HDC CanvasSkia::BeginPlatformPaint()
    {
        return skia::BeginPlatformPaint(this);
    }

    void CanvasSkia::EndPlatformPaint()
    {
        skia::EndPlatformPaint(this);
    }

    void CanvasSkia::ConcatTransform(const Transform& transform)
    {
        concat(transform.matrix());
    }

    CanvasSkia* CanvasSkia::AsCanvasSkia()
    {
        return this;
    }

    const CanvasSkia* CanvasSkia::AsCanvasSkia() const
    {
        return this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // CanvasSkia, private:

    bool CanvasSkia::IntersectsClipRectInt(int x, int y, int w, int h)
    {
        SkRect clip;
        return getClipBounds(&clip) &&
            clip.intersect(SkIntToScalar(x), SkIntToScalar(y),
                SkIntToScalar(x + w), SkIntToScalar(y + h));
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Canvas, public:

    Canvas* Canvas::CreateCanvas()
    {
        return new CanvasSkia;
    }

    Canvas* Canvas::CreateCanvas(int width, int height, bool is_opaque)
    {
        return new CanvasSkia(width, height, is_opaque);
    }


    class CanvasPaintWin : public CanvasSkiaPaint, public CanvasPaint
    {
    public:
        CanvasPaintWin(HWND view) : CanvasSkiaPaint(view) {}

        virtual bool IsValid() const
        {
            return isEmpty();
        }

        virtual Rect GetInvalidRect() const
        {
            return Rect(paintStruct().rcPaint);
        }

        virtual Canvas* AsCanvas()
        {
            return this;
        }
    };

    CanvasPaint* CanvasPaint::CreateCanvasPaint(HWND view)
    {
        return new CanvasPaintWin(view);
    }

} //namespace gfx
#endif 

