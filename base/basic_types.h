#ifndef __base_basic_types_h__
#define __base_basic_types_h__

#include <stddef.h>

#include "port.h"

typedef signed char         schar;
typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
#if __LP64__
typedef long                int64;
#else
typedef long long           int64;
#endif
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
#if __LP64__
typedef unsigned long       uint64;
#else
typedef unsigned long long  uint64;
#endif
typedef signed int          char32;

const uint8  kuint8max = ((uint8)0xFF);
const uint16 kuint16max = ((uint16)0xFFFF);
const uint32 kuint32max = ((uint32)0xFFFFFFFF);
const uint64 kuint64max = ((uint64)GG_LONGLONG(0xFFFFFFFFFFFFFFFF));
const  int8  kint8min = ((int8)0x80);
const  int8  kint8max = ((int8)0x7F);
const  int16 kint16min = ((int16)0x8000);
const  int16 kint16max = ((int16)0x7FFF);
const  int32 kint32min = ((int32)0x80000000);
const  int32 kint32max = ((int32)0x7FFFFFFF);
const  int64 kint64min = ((int64)GG_LONGLONG(0x8000000000000000));
const  int64 kint64max = ((int64)GG_LONGLONG(0x7FFFFFFFFFFFFFFF));

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName(); \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

template<typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define ARRAYSIZE_UNSAFE(a) \
    ((sizeof(a)/sizeof(*(a))) / \
    static_cast<size_t>(!(sizeof(a)%sizeof(*(a)))))

template<bool>
struct CompileAssert {};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr)?1:-1]

template<class Dest, class Source>
inline Dest bit_cast(const Source& source)
{
    typedef char VerifySizesAreEqual[sizeof(Dest) == sizeof(Source) ? 1 : -1];

    Dest dest;
    memcpy(&dest, &source, sizeof(dest));
    return dest;
}

namespace base
{
    enum LinkerInitialized { LINKER_INITIALIZED };
} //namespace base


#endif //__base_basic_types_h__