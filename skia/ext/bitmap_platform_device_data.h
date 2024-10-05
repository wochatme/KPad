#ifndef __skia_bitmap_platform_device_data_h__
#define __skia_bitmap_platform_device_data_h__

#include "bitmap_platform_device_win.h"

namespace skia
{
    class BitmapPlatformDevice::BitmapPlatformDeviceData : public SkRefCnt
    {
    public:
        explicit BitmapPlatformDeviceData(HBITMAP bitmap);

        HDC GetBitmapDC();
        void ReleaseBitmapDC();
        bool IsBitmapDCCreated() const;

        void SetMatrixClip(const SkMatrix& transform, const SkRegion& region);

        void LoadConfig();

        const SkMatrix& transform() const
        {
            return transform_;
        }

        HBITMAP bitmap_context()
        {
            return bitmap_context_;
        }

    private:
        virtual ~BitmapPlatformDeviceData();

        HBITMAP bitmap_context_;

        HDC hdc_;

        bool config_dirty_;

        SkMatrix transform_;

        SkRegion clip_region_;

        BitmapPlatformDeviceData(const BitmapPlatformDeviceData&);
        BitmapPlatformDeviceData& operator=(const BitmapPlatformDeviceData&);
    };

} //namespace skia

#endif //__skia_bitmap_platform_device_data_h__