#ifndef __base_thread_local_h__
#define __base_thread_local_h__

#include "base/basic_types.h"

namespace base
{
    namespace internal
    {
        struct ThreadLocalPlatform
        {
            typedef unsigned long SlotType;

            static void AllocateSlot(SlotType& slot);
            static void FreeSlot(SlotType& slot);
            static void* GetValueFromSlot(SlotType& slot);
            static void SetValueInSlot(SlotType& slot, void* value);
        };

    } //namespace internal

    template<typename Type>
    class ThreadLocalPointer
    {
    public:
        ThreadLocalPointer() : slot_()
        {
            internal::ThreadLocalPlatform::AllocateSlot(slot_);
        }

        ~ThreadLocalPointer()
        {
            internal::ThreadLocalPlatform::FreeSlot(slot_);
        }

        Type* Get()
        {
            return static_cast<Type*>(
                internal::ThreadLocalPlatform::GetValueFromSlot(slot_));
        }

        void Set(Type* ptr)
        {
            internal::ThreadLocalPlatform::SetValueInSlot(slot_, ptr);
        }

    private:
        typedef internal::ThreadLocalPlatform::SlotType SlotType;

        SlotType slot_;

        DISALLOW_COPY_AND_ASSIGN(ThreadLocalPointer<Type>);
    };

    class ThreadLocalBoolean
    {
    public:
        ThreadLocalBoolean() {}
        ~ThreadLocalBoolean() {}

        bool Get()
        {
            return tlp_.Get() != NULL;
        }

        void Set(bool val)
        {
            tlp_.Set(val ? this : NULL);
        }

    private:
        ThreadLocalPointer<void> tlp_;

        DISALLOW_COPY_AND_ASSIGN(ThreadLocalBoolean);
    };

} //namespace base

#endif //__base_thread_local_h__