#ifndef __base_lazy_instance_h__
#define __base_lazy_instance_h__

#include <new> // placement new.

#include "atomicops.h"
#include "threading/thread_restrictions.h"

namespace base
{
    template<typename Type>
    struct DefaultLazyInstanceTraits
    {
        static const bool kAllowedToAccessOnNonjoinableThread = false;

        static Type* New(void* instance)
        {
            return new (instance) Type();
        }
        static void Delete(void* instance)
        {
            reinterpret_cast<Type*>(instance)->~Type();
        }
    };

    template<typename Type>
    struct LeakyLazyInstanceTraits
    {
        static const bool kAllowedToAccessOnNonjoinableThread = true;

        static Type* New(void* instance)
        {
            return DefaultLazyInstanceTraits<Type>::New(instance);
        }
        static void (*Delete)(void* instance);
    };

    template<typename Type>
    void (*LeakyLazyInstanceTraits<Type>::Delete)(void* instance) = NULL;

    class LazyInstanceHelper
    {
    protected:
        enum
        {
            STATE_EMPTY = 0,
            STATE_CREATING = 1,
            STATE_CREATED = 2
        };

        explicit LazyInstanceHelper(LinkerInitialized x) { /* state_ is 0 */ }

        bool NeedsInstance();

        void CompleteInstance(void* instance, void (*dtor)(void*));

        subtle::Atomic32 state_;

    private:
        DISALLOW_COPY_AND_ASSIGN(LazyInstanceHelper);
    };

    template<typename Type, typename Traits = DefaultLazyInstanceTraits<Type> >
    class LazyInstance : public LazyInstanceHelper
    {
    public:
        explicit LazyInstance(LinkerInitialized x) : LazyInstanceHelper(x) {}

        Type& Get()
        {
            return *Pointer();
        }

        Type* Pointer()
        {
            if (!Traits::kAllowedToAccessOnNonjoinableThread)
            {
                ThreadRestrictions::AssertSingletonAllowed();
            }

            if ((subtle::NoBarrier_Load(&state_) != STATE_CREATED) && NeedsInstance())
            {
                instance_ = Traits::New(buf_);

                void (*dtor)(void*) = Traits::Delete;
                CompleteInstance(this, (dtor == NULL) ? NULL : OnExit);
            }

            return instance_;
        }

        bool operator==(Type* p)
        {
            switch (subtle::NoBarrier_Load(&state_))
            {
            case STATE_EMPTY:
                return p == NULL;
            case STATE_CREATING:
                return static_cast<int8*>(static_cast<void*>(p)) == buf_;
            case STATE_CREATED:
                return p == instance_;
            default:
                return false;
            }
        }

    private:
        static void OnExit(void* lazy_instance)
        {
            LazyInstance<Type, Traits>* me =
                reinterpret_cast<LazyInstance<Type, Traits>*>(lazy_instance);
            Traits::Delete(me->instance_);
            me->instance_ = NULL;
            subtle::Release_Store(&me->state_, STATE_EMPTY);
        }

        int8 buf_[sizeof(Type)]; 
        Type* instance_;

        DISALLOW_COPY_AND_ASSIGN(LazyInstance);
    };

} //namespace base

#endif //__base_lazy_instance_h__