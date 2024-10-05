#ifndef __base__ref_counted_memory_h__
#define __base__ref_counted_memory_h__

#include <string>
#include <vector>

#include "ref_counted.h"

class RefCountedMemory : public base::RefCountedThreadSafe<RefCountedMemory>
{
public:
    virtual const unsigned char* front() const = 0;

    virtual size_t size() const = 0;

protected:
    friend class base::RefCountedThreadSafe<RefCountedMemory>;
    RefCountedMemory();
    virtual ~RefCountedMemory();
};

class RefCountedStaticMemory : public RefCountedMemory
{
public:
    RefCountedStaticMemory() : data_(NULL), length_(0) {}
    RefCountedStaticMemory(const unsigned char* data, size_t length)
        : data_(length ? data : NULL), length_(length) {}

    virtual const unsigned char* front() const;
    virtual size_t size() const;

private:
    const unsigned char* data_;
    size_t length_;

    DISALLOW_COPY_AND_ASSIGN(RefCountedStaticMemory);
};

class RefCountedBytes : public RefCountedMemory
{
public:
    RefCountedBytes();

    RefCountedBytes(const std::vector<unsigned char>& initializer);

    static RefCountedBytes* TakeVector(std::vector<unsigned char>* to_destroy);

    virtual const unsigned char* front() const;
    virtual size_t size() const;

    const std::vector<unsigned char>& data() const { return data_; }
    std::vector<unsigned char>& data() { return data_; }

private:
    friend class base::RefCountedThreadSafe<RefCountedBytes>;
    virtual ~RefCountedBytes();

    std::vector<unsigned char> data_;

    DISALLOW_COPY_AND_ASSIGN(RefCountedBytes);
};

namespace base
{
    // An implementation of RefCountedMemory, where the bytes are stored in an STL
    // string. Use this if your data naturally arrives in that format.
    class RefCountedString : public RefCountedMemory
    {
    public:
        RefCountedString();

        // Constructs a RefCountedString object by performing a swap. (To non
        // destructively build a RefCountedString, use the default constructor and
        // copy into object->data()).
        static RefCountedString* TakeString(std::string* to_destroy);

        // Overridden from RefCountedMemory:
        virtual const unsigned char* front() const;
        virtual size_t size() const;

        const std::string& data() const { return data_; }
        std::string& data() { return data_; }

    private:
        friend class base::RefCountedThreadSafe<RefCountedString>;
        virtual ~RefCountedString();

        std::string data_;

        DISALLOW_COPY_AND_ASSIGN(RefCountedString);
    };

} //namespace base

#endif //__base__ref_counted_memory_h__