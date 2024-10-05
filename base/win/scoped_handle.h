#ifndef __base_scoped_handle_h__
#define __base_scoped_handle_h__

#include <windows.h>

#include "base/logging.h"

namespace base
{
    namespace win
    {
        class ScopedHandle
        {
        public:
            ScopedHandle() : handle_(NULL) {}

            explicit ScopedHandle(HANDLE h) : handle_(NULL)
            {
                Set(h);
            }

            ~ScopedHandle()
            {
                Close();
            }

            bool IsValid() const
            {
                return handle_ != NULL;
            }

            void Set(HANDLE new_handle)
            {
                Close();

                if (new_handle != INVALID_HANDLE_VALUE)
                {
                    handle_ = new_handle;
                }
            }

            HANDLE Get()
            {
                return handle_;
            }

            operator HANDLE() { return handle_; }

            HANDLE Take()
            {
                HANDLE h = handle_;
                handle_ = NULL;
                return h;
            }

            void Close()
            {
                if (handle_)
                {
                    if (!CloseHandle(handle_))
                    {
                        NOTREACHED();
                    }
                    handle_ = NULL;
                }
            }

        private:
            HANDLE handle_;
            DISALLOW_COPY_AND_ASSIGN(ScopedHandle);
        };

    } //namespace win
} //namespace base

#endif //__base_scoped_handle_h__