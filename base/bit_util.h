#ifndef __base_bit_util_h__
#define __base_bit_util_h__

#include "logging.h"

namespace base
{
    namespace bits
    {
        inline int Log2Floor(uint32 n)
        {
            if (n == 0)
            {
                return -1;
            }
            int log = 0;
            uint32 value = n;
            for (int i = 4; i >= 0; --i)
            {
                int shift = (1 << i);
                uint32 x = value >> shift;
                if (x != 0)
                {
                    value = x;
                    log += shift;
                }
            }
            DCHECK_EQ(value, 1u);
            return log;
        }

        inline int Log2Ceiling(uint32 n)
        {
            if (n == 0)
            {
                return -1;
            }
            else
            {
                return 1 + Log2Floor(n - 1);
            }
        }
    } //namespace bits
} //namespace base

#endif //__base_bit_util_h__