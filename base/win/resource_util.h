#ifndef __base_resource_util_h__
#define __base_resource_util_h__

#include <windows.h>

namespace base
{
    namespace win
    {

        bool GetDataResourceFromModule(HMODULE module, int resource_id,
            void** data, size_t* length);

    } //namespace win
} //namespace base

#endif //__base_resource_util_h__