#ifndef __base_rand_util_h__
#define __base_rand_util_h__

#include <string>
#include "basic_types.h"

namespace base
{
    uint64 RandUint64();

    int RandInt(int min, int max);

    uint64 RandGenerator(uint64 range);

    double RandDouble();

    // Given input |bits|, convert with maximum precision to a double in
    // the range [0, 1). Thread-safe.
    double BitsToOpenEndedUnitInterval(uint64 bits);

    // Fills |output_length| bytes of |output| with cryptographically strong random
    // data.
    void RandBytes(void* output, size_t output_length);

    // Fills a string of length |length| with with cryptographically strong random
    // data and returns it.
    //
    // Not that this is a variation of |RandBytes| with a different return type.
    std::string RandBytesAsString(size_t length);

    // Generate128BitRandomBase64String returns a string of length 24 containing
    // cryptographically strong random data encoded in base64.
    std::string Generate128BitRandomBase64String();
    std::string Generate256BitRandomBase64String();

} //namespace base


#endif //__base_rand_util_h__