#ifndef __base_scoped_gdi_object_h__
#define __base_scoped_gdi_object_h__

#include <windows.h>
#include "base/basic_types.h"

namespace base
{
    namespace win
    {

        template<class T>
        class ScopedGDIObject
        {
        public:
            ScopedGDIObject() : object_(NULL) {}
            explicit ScopedGDIObject(T object) : object_(object) {}

            ~ScopedGDIObject()
            {
                Close();
            }

            T Get()
            {
                return object_;
            }

            void Set(T object)
            {
                if (object_ && object != object_)
                {
                    Close();
                }
                object_ = object;
            }

            ScopedGDIObject& operator=(T object)
            {
                Set(object);
                return *this;
            }

            T release()
            {
                T object = object_;
                object_ = NULL;
                return object;
            }

            operator T() { return object_; }

        private:
            void Close()
            {
                if (object_)
                {
                    DeleteObject(object_);
                }
            }

            T object_;
            DISALLOW_COPY_AND_ASSIGN(ScopedGDIObject);
        };

        template<>
        void ScopedGDIObject<HICON>::Close()
        {
            if (object_)
            {
                DestroyIcon(object_);
            }
        }

        typedef ScopedGDIObject<HBITMAP> ScopedBitmap;
        typedef ScopedGDIObject<HRGN> ScopedRegion;
        typedef ScopedGDIObject<HFONT> ScopedHFONT;
        typedef ScopedGDIObject<HICON> ScopedHICON;

    } //namespace win
} //namespace base

#endif //__base_scoped_gdi_object_h__