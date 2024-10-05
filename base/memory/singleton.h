#ifndef __base_singleton_h__
#define __base_singleton_h__

#include "base/at_exit.h"
#include "base/atomicops.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/platform_thread.h"

template<typename Type>
struct DefaultSingletonTraits
{
    static Type* New()
    {
        return new Type();
    }

    static void Delete(Type* x)
    {
        delete x;
    }

    static const bool kRegisterAtExit = true;

    static const bool kAllowedToAccessOnNonjoinableThread = false;
};


template<typename Type>
struct LeakySingletonTraits : public DefaultSingletonTraits<Type>
{
    static const bool kRegisterAtExit = false;
    static const bool kAllowedToAccessOnNonjoinableThread = true;
};

template<typename Type>
struct StaticMemorySingletonTraits
{
    static Type* New()
    {
        if (base::subtle::NoBarrier_AtomicExchange(&dead_, 1))
        {
            return NULL;
        }
        Type* ptr = reinterpret_cast<Type*>(buffer_);

        new(ptr) Type();
        return ptr;
    }

    static void Delete(Type* p)
    {
        base::subtle::NoBarrier_Store(&dead_, 1);
        base::subtle::MemoryBarrier();
        if (p != NULL)
        {
            p->Type::~Type();
        }
    }

    static const bool kRegisterAtExit = true;
    static const bool kAllowedToAccessOnNonjoinableThread = true;

    static void Resurrect()
    {
        base::subtle::NoBarrier_Store(&dead_, 0);
    }

private:
    static const size_t kBufferSize = (sizeof(Type) +
        sizeof(intptr_t) - 1) / sizeof(intptr_t);
    static intptr_t buffer_[kBufferSize];

    static base::subtle::Atomic32 dead_;
};

template<typename Type> intptr_t
StaticMemorySingletonTraits<Type>::buffer_[kBufferSize];
template<typename Type> base::subtle::Atomic32
StaticMemorySingletonTraits<Type>::dead_ = 0;

template<typename Type, typename Traits = DefaultSingletonTraits<Type>,
    typename DifferentiatingType = Type>
class Singleton
{
public:
    friend Type* Type::GetInstance();

    static Type* get()
    {
        if (!Traits::kAllowedToAccessOnNonjoinableThread)
        {
            base::ThreadRestrictions::AssertSingletonAllowed();
        }

        static const base::subtle::AtomicWord kBeingCreatedMarker = 1;

        base::subtle::AtomicWord value = base::subtle::NoBarrier_Load(&instance_);
        if (value != 0 && value != kBeingCreatedMarker)
        {
            return reinterpret_cast<Type*>(value);
        }

        if (base::subtle::Acquire_CompareAndSwap(&instance_, 0,
            kBeingCreatedMarker) == 0)
        {
            Type* newval = Traits::New();

            base::subtle::Release_Store(&instance_,
                reinterpret_cast<base::subtle::AtomicWord>(newval));

            if (newval != NULL && Traits::kRegisterAtExit)
            {
                base::AtExitManager::RegisterCallback(OnExit, NULL);
            }

            return newval;
        }

        while (true)
        {
            value = base::subtle::NoBarrier_Load(&instance_);
            if (value != kBeingCreatedMarker)
            {
                break;
            }
            base::PlatformThread::YieldCurrentThread();
        }

        return reinterpret_cast<Type*>(value);
    }

private:
    static void OnExit(void* /*unused*/)
    {
        Traits::Delete(
            reinterpret_cast<Type*>(base::subtle::NoBarrier_Load(&instance_)));
        instance_ = 0;
    }
    static base::subtle::AtomicWord instance_;
};

template<typename Type, typename Traits, typename DifferentiatingType>
base::subtle::AtomicWord Singleton<Type, Traits, DifferentiatingType>::
instance_ = 0;

#endif //__base_singleton_h__