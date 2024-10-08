#ifndef __base_atomic_ref_count_h__
#define __base_atomic_ref_count_h__

#include "atomicops.h"

namespace base
{
    typedef subtle::Atomic32 AtomicRefCount;

    inline void AtomicRefCountIncN(volatile AtomicRefCount* ptr,
        AtomicRefCount increment)
    {
        subtle::NoBarrier_AtomicIncrement(ptr, increment);
    }

    inline bool AtomicRefCountDecN(volatile AtomicRefCount* ptr,
        AtomicRefCount decrement)
    {
        bool res = subtle::Barrier_AtomicIncrement(ptr, -decrement) != 0;
        return res;
    }

    inline void AtomicRefCountInc(volatile AtomicRefCount* ptr)
    {
        AtomicRefCountIncN(ptr, 1);
    }

    inline bool AtomicRefCountDec(volatile AtomicRefCount* ptr)
    {
        return AtomicRefCountDecN(ptr, 1);
    }

    inline bool AtomicRefCountIsOne(volatile AtomicRefCount* ptr)
    {
        bool res = subtle::Acquire_Load(ptr) == 1;
        return res;
    }

    inline bool AtomicRefCountIsZero(volatile AtomicRefCount* ptr)
    {
        bool res = subtle::Acquire_Load(ptr) == 0;
        return res;
    }

} //namespace base


#endif //__base_atomic_ref_count_h__