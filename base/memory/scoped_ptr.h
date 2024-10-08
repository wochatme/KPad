// Usage Example (scoped_ptr):
//     {
//       scoped_ptr<Foo> foo(new Foo("wee"));
//     } // foo goes out of scope, releasing the pointer with it.
//
//     {
//       scoped_ptr<Foo> foo;          // No pointer managed.
//       foo.reset(new Foo("wee"));    // Now a pointer is managed.
//       foo.reset(new Foo("wee2"));   // Foo("wee") was destroyed.
//       foo.reset(new Foo("wee3"));   // Foo("wee2") was destroyed.
//       foo->Method();                // Foo::Method() called.
//       foo.get()->Method();          // Foo::Method() called.
//       SomeFunc(foo.release());      // SomeFunc takes ownership, foo no longer
//                                     // manages a pointer.
//       foo.reset(new Foo("wee4"));   // foo manages a pointer again.
//       foo.reset();                  // Foo("wee4") destroyed, foo no longer
//                                     // manages a pointer.
//     } // foo wasn't managing a pointer, so nothing was destroyed.
//
// Usage Example(scoped_array):
//     {
//       scoped_array<Foo> foo(new Foo[100]);
//       foo.get()->Method();  // Foo::Method on the 0th element.
//       foo[10].Method();     // Foo::Method on the 10th element.
//     }

#ifndef __base_scoped_ptr_h__
#define __base_scoped_ptr_h__

#pragma once

#include <assert.h>
#include <cstddef>
#include <stdlib.h>

template<class C>
class scoped_ptr
{
public:
    typedef C element_type;

    explicit scoped_ptr(C* p = NULL) : ptr_(p) {}

    ~scoped_ptr()
    {
        enum { type_must_be_complete = sizeof(C) };
        delete ptr_;
    }

    void reset(C* p = NULL)
    {
        if (p != ptr_)
        {
            enum { type_must_be_complete = sizeof(C) };
            delete ptr_;
            ptr_ = p;
        }
    }

    C& operator*() const
    {
        assert(ptr_ != NULL);
        return *ptr_;
    }
    C* operator->() const
    {
        assert(ptr_ != NULL);
        return ptr_;
    }
    C* get() const { return ptr_; }

    bool operator==(C* p) const { return ptr_ == p; }
    bool operator!=(C* p) const { return ptr_ != p; }

    void swap(scoped_ptr& p2)
    {
        C* tmp = ptr_;
        ptr_ = p2.ptr_;
        p2.ptr_ = tmp;
    }

    C* release()
    {
        C* retVal = ptr_;
        ptr_ = NULL;
        return retVal;
    }

private:
    C* ptr_;

    template<class C2> bool operator==(scoped_ptr<C2> const& p2) const;
    template<class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

    scoped_ptr(const scoped_ptr&);
    void operator=(const scoped_ptr&);
};

template<class C>
void swap(scoped_ptr<C>& p1, scoped_ptr<C>& p2)
{
    p1.swap(p2);
}

template<class C>
bool operator==(C* p1, const scoped_ptr<C>& p2)
{
    return p1 == p2.get();
}

template<class C>
bool operator!=(C* p1, const scoped_ptr<C>& p2)
{
    return p1 != p2.get();
}

template<class C>
class scoped_array
{
public:
    typedef C element_type;

    explicit scoped_array(C* p = NULL) : array_(p) { }

    ~scoped_array()
    {
        enum { type_must_be_complete = sizeof(C) };
        delete[] array_;
    }

    void reset(C* p = NULL)
    {
        if (p != array_)
        {
            enum { type_must_be_complete = sizeof(C) };
            delete[] array_;
            array_ = p;
        }
    }

    C& operator[](std::ptrdiff_t i) const
    {
        assert(i >= 0);
        assert(array_ != NULL);
        return array_[i];
    }

    C* get() const
    {
        return array_;
    }

    bool operator==(C* p) const { return array_ == p; }
    bool operator!=(C* p) const { return array_ != p; }

    void swap(scoped_array& p2)
    {
        C* tmp = array_;
        array_ = p2.array_;
        p2.array_ = tmp;
    }

    C* release()
    {
        C* retVal = array_;
        array_ = NULL;
        return retVal;
    }

private:
    C* array_;

    template<class C2> bool operator==(scoped_array<C2> const& p2) const;
    template<class C2> bool operator!=(scoped_array<C2> const& p2) const;

    scoped_array(const scoped_array&);
    void operator=(const scoped_array&);
};

template<class C>
void swap(scoped_array<C>& p1, scoped_array<C>& p2)
{
    p1.swap(p2);
}

template<class C>
bool operator==(C* p1, const scoped_array<C>& p2)
{
    return p1 == p2.get();
}

template<class C>
bool operator!=(C* p1, const scoped_array<C>& p2)
{
    return p1 != p2.get();
}

class ScopedPtrMallocFree
{
public:
    inline void operator()(void* x) const
    {
        free(x);
    }
};

template<class C, class FreeProc = ScopedPtrMallocFree>
class scoped_ptr_malloc
{
public:
    typedef C element_type;

    explicit scoped_ptr_malloc(C* p = NULL) : ptr_(p) {}

    ~scoped_ptr_malloc()
    {
        reset();
    }

    void reset(C* p = NULL)
    {
        if (ptr_ != p)
        {
            FreeProc free_proc;
            free_proc(ptr_);
            ptr_ = p;
        }
    }

    C& operator*() const
    {
        assert(ptr_ != NULL);
        return *ptr_;
    }

    C* operator->() const
    {
        assert(ptr_ != NULL);
        return ptr_;
    }

    C* get() const
    {
        return ptr_;
    }

    bool operator==(C* p) const
    {
        return ptr_ == p;
    }

    bool operator!=(C* p) const
    {
        return ptr_ != p;
    }

    void swap(scoped_ptr_malloc& b)
    {
        C* tmp = b.ptr_;
        b.ptr_ = ptr_;
        ptr_ = tmp;
    }

    C* release()
    {
        C* tmp = ptr_;
        ptr_ = NULL;
        return tmp;
    }

private:
    C* ptr_;

    template<class C2, class GP>
    bool operator==(scoped_ptr_malloc<C2, GP> const& p) const;
    template<class C2, class GP>
    bool operator!=(scoped_ptr_malloc<C2, GP> const& p) const;

    scoped_ptr_malloc(const scoped_ptr_malloc&);
    void operator=(const scoped_ptr_malloc&);
};

template<class C, class FP> inline
void swap(scoped_ptr_malloc<C, FP>& a, scoped_ptr_malloc<C, FP>& b)
{
    a.swap(b);
}

template<class C, class FP> inline
bool operator==(C* p, const scoped_ptr_malloc<C, FP>& b)
{
    return p == b.get();
}

template<class C, class FP> inline
bool operator!=(C* p, const scoped_ptr_malloc<C, FP>& b)
{
    return p != b.get();
}

#endif //__base_scoped_ptr_h__