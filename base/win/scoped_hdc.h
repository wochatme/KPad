#ifndef __base_scoped_hdc_h__
#define __base_scoped_hdc_h__

#include <windows.h>
#include "base/basic_types.h"

namespace base
{
    namespace win
    {
        class ScopedHDC
        {
        public:
            ScopedHDC() : hdc_(NULL) {}
            explicit ScopedHDC(HDC h) : hdc_(h) {}

            ~ScopedHDC()
            {
                Close();
            }

            HDC Get()
            {
                return hdc_;
            }

            void Set(HDC h)
            {
                Close();
                hdc_ = h;
            }

            operator HDC() { return hdc_; }

        private:
            void Close()
            {
#ifdef NOGDI
                assert(false);
#else
                if (hdc_)
                {
                    DeleteDC(hdc_);
                }
#endif //NOGDI
            }

            HDC hdc_;
            DISALLOW_COPY_AND_ASSIGN(ScopedHDC);
        };

    } //namespace win
} //namespace base

#endif //__base_scoped_hdc_h__