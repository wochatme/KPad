#ifndef __ui_gfx_canvas_paint_win_h__
#define __ui_gfx_canvas_paint_win_h__

#include "skia/ext/platform_canvas.h"

namespace gfx
{
    //     case WM_PAINT: {
    //       skia::PlatformCanvasPaint canvas(hwnd);
    //       if(!canvas.isEmpty()) {
    //         ... paint to the canvas ...
    //       }
    //       return 0;
    //     }
    template<class T>
    class CanvasPaintT : public T
    {
    public:
        explicit CanvasPaintT(HWND hwnd) : hwnd_(hwnd), paint_dc_(NULL),
            for_paint_(true)
        {
            memset(&ps_, 0, sizeof(ps_));
            initPaint(true);
        }

        CanvasPaintT(HWND hwnd, bool opaque) : hwnd_(hwnd), paint_dc_(NULL),
            for_paint_(true)
        {
            memset(&ps_, 0, sizeof(ps_));
            initPaint(opaque);
        }

        CanvasPaintT(HDC dc, bool opaque, int x, int y, int w, int h)
            : hwnd_(NULL), paint_dc_(dc), for_paint_(false)
        {
            memset(&ps_, 0, sizeof(ps_));
            ps_.rcPaint.left = x;
            ps_.rcPaint.right = x + w;
            ps_.rcPaint.top = y;
            ps_.rcPaint.bottom = y + h;
            init(opaque);
        }

        virtual ~CanvasPaintT()
        {
            if (!isEmpty())
            {
                restoreToCount(1);

                skia::DrawToNativeContext(this, paint_dc_, ps_.rcPaint.left,
                    ps_.rcPaint.top, NULL);
            }
            if (for_paint_)
            {
                EndPaint(hwnd_, &ps_);
            }
        }

        bool isEmpty() const
        {
            return ps_.rcPaint.right - ps_.rcPaint.left == 0 ||
                ps_.rcPaint.bottom - ps_.rcPaint.top == 0;
        }

        // paintstruct().rcPaint.
        const PAINTSTRUCT& paintStruct() const
        {
            return ps_;
        }

        HDC paintDC() const
        {
            return paint_dc_;
        }

    protected:
        HWND hwnd_;
        HDC paint_dc_;
        PAINTSTRUCT ps_;

    private:
        void initPaint(bool opaque)
        {
            paint_dc_ = BeginPaint(hwnd_, &ps_);

            init(opaque);
        }

        void init(bool opaque)
        {
            const int width = ps_.rcPaint.right - ps_.rcPaint.left;
            const int height = ps_.rcPaint.bottom - ps_.rcPaint.top + 1;
            if (!initialize(width, height, opaque, NULL))
            {
                *(char*)0 = 0;
            }

            translate(SkIntToScalar(-ps_.rcPaint.left),
                SkIntToScalar(-ps_.rcPaint.top));
        }

        const bool for_paint_;

        CanvasPaintT(const CanvasPaintT&);
        CanvasPaintT& operator=(const CanvasPaintT&);
    };

    typedef CanvasPaintT<skia::PlatformCanvas> PlatformCanvasPaint;

} //namespace gfx

#endif //__ui_gfx_canvas_paint_win_h__