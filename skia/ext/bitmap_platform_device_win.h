#ifndef __skia_bitmap_platform_device_win_h__
#define __skia_bitmap_platform_device_win_h__

#include "platform_device_win.h"

namespace skia
{
    class BitmapPlatformDevice : public PlatformDevice, public SkDevice
    {
    public:
        static BitmapPlatformDevice* create(HDC screen_dc, int width, int height,
            bool is_opaque, HANDLE shared_section);

        static BitmapPlatformDevice* create(int width, int height, bool is_opaque,
            HANDLE shared_section);

        virtual ~BitmapPlatformDevice();

        // Retrieves the bitmap DC, which is the memory DC for our bitmap data. The
        // bitmap DC is lazy created.
        virtual HDC BeginPlatformPaint();
        virtual void EndPlatformPaint();

        virtual void DrawToNativeContext(HDC dc, int x, int y, const RECT* src_rect);

        virtual void setMatrixClip(const SkMatrix& transform,
            const SkRegion& region, const SkClipStack&);

    protected:
        virtual void onAccessBitmap(SkBitmap* bitmap);

        virtual SkDevice* onCreateCompatibleDevice(SkBitmap::Config, int width,
            int height, bool isOpaque);

    private:
        class BitmapPlatformDeviceData;

        BitmapPlatformDevice(BitmapPlatformDeviceData* data,
            const SkBitmap& bitmap);

        BitmapPlatformDeviceData* data_;
    };

} //namespace skia

#endif //__skia_bitmap_platform_device_win_h__