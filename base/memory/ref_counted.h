#ifndef __base_ref_counted_h__
#define __base_ref_counted_h__

#include "base/atomic_ref_count.h"

namespace base
{
    namespace subtle
    {
        class RefCountedBase
        {
        public:
            static bool ImplementsThreadSafeReferenceCounting() { return false; }
            bool HasOneRef() const { return ref_count_ == 1; }

        protected:
            RefCountedBase();
            ~RefCountedBase();

            void AddRef() const;

            bool Release() const;

        private:
            mutable int ref_count_;
#ifndef NDEBUG
            mutable bool in_dtor_;
#endif
            DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
        };

        class RefCountedThreadSafeBase
        {
        public:
            static bool ImplementsThreadSafeReferenceCounting() { return true; }
            bool HasOneRef() const;

        protected:
            RefCountedThreadSafeBase();
            ~RefCountedThreadSafeBase();

            void AddRef() const;

            bool Release() const;

        private:
            mutable AtomicRefCount ref_count_;
#ifndef NDEBUG
            mutable bool in_dtor_;
#endif

            DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafeBase);
        };

    } //namespace subtle

    template<class T>
    class RefCounted : public subtle::RefCountedBase
    {
    public:
        RefCounted() {}
        ~RefCounted() {}

        void AddRef() const
        {
            subtle::RefCountedBase::AddRef();
        }

        void Release() const
        {
            if (subtle::RefCountedBase::Release())
            {
                delete static_cast<const T*>(this);
            }
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(RefCounted<T>);
    };

    template<class T, typename Traits> class RefCountedThreadSafe;

    template<typename T>
    struct DefaultRefCountedThreadSafeTraits
    {
        static void Destruct(const T* x)
        {
            RefCountedThreadSafe<T,
                DefaultRefCountedThreadSafeTraits>::DeleteInternal(x);
        }
    };

    // A thread-safe variant of RefCounted<T>
    //
    //   class MyFoo : public base::RefCountedThreadSafe<MyFoo> {
    //    ...
    //   };
    //
    // If you're using the default trait, then you should add compile time
    // asserts that no one else is deleting your object.  i.e.
    //    private:
    //     friend class base::RefCountedThreadSafe<MyFoo>;
    //     ~MyFoo();
    template<class T, typename Traits = DefaultRefCountedThreadSafeTraits<T> >
    class RefCountedThreadSafe : public subtle::RefCountedThreadSafeBase
    {
    public:
        RefCountedThreadSafe() {}
        ~RefCountedThreadSafe() {}

        void AddRef() const
        {
            subtle::RefCountedThreadSafeBase::AddRef();
        }

        void Release() const
        {
            if (subtle::RefCountedThreadSafeBase::Release())
            {
                Traits::Destruct(static_cast<const T*>(this));
            }
        }

    private:
        friend struct DefaultRefCountedThreadSafeTraits<T>;
        static void DeleteInternal(const T* x) { delete x; }

        DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafe);
    };

    template<typename T>
    class RefCountedData : public base::RefCounted< base::RefCountedData<T> >
    {
    public:
        RefCountedData() : data() {}
        RefCountedData(const T& in_value) : data(in_value) {}

        T data;
    };

} //namespace base

template<class T>
class scoped_refptr
{
public:
    scoped_refptr() : ptr_(NULL) {}

    scoped_refptr(T* p) : ptr_(p)
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_)
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    template<typename U>
    scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get())
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    ~scoped_refptr()
    {
        if (ptr_)
        {
            ptr_->Release();
        }
    }

    T* get() const { return ptr_; }
    operator T* () const { return ptr_; }
    T* operator->() const { return ptr_; }

    T* release()
    {
        T* retVal = ptr_;
        ptr_ = NULL;
        return retVal;
    }

    scoped_refptr<T>& operator=(T* p)
    {
        if (p)
        {
            p->AddRef();
        }
        if (ptr_)
        {
            ptr_->Release();
        }
        ptr_ = p;
        return *this;
    }

    scoped_refptr<T>& operator=(const scoped_refptr<T>& r)
    {
        return *this = r.ptr_;
    }

    template<typename U>
    scoped_refptr<T>& operator=(const scoped_refptr<U>& r)
    {
        return *this = r.get();
    }

    void swap(T** pp)
    {
        T* p = ptr_;
        ptr_ = *pp;
        *pp = p;
    }

    void swap(scoped_refptr<T>& r)
    {
        swap(&r.ptr_);
    }

protected:
    T* ptr_;
};

template<typename T>
scoped_refptr<T> make_scoped_refptr(T* t)
{
    return scoped_refptr<T>(t);
}


#endif //__base_ref_counted_h__