#include "stringprintf.h"

#include <vector>

#include "string_util.h"

namespace base
{
    inline int vsnprintfT(char* buffer, size_t buf_size,
        const char* format, va_list argptr)
    {
        return base::vsnprintf(buffer, buf_size, format, argptr);
    }

    inline int vsnprintfT(wchar_t* buffer, size_t buf_size,
        const wchar_t* format, va_list argptr)
    {
        return base::vswprintf(buffer, buf_size, format, argptr);
    }

    template<typename StringType>
    static void StringAppendVT(StringType* dst,
        const typename StringType::value_type* format, va_list ap)
    {
        typename StringType::value_type stack_buf[1024];

        va_list ap_copy;
        GG_VA_COPY(ap_copy, ap);

        int result = vsnprintfT(stack_buf, arraysize(stack_buf), format, ap_copy);
        va_end(ap_copy);

        if (result >= 0 && result < static_cast<int>(arraysize(stack_buf)))
        {
            dst->append(stack_buf, result);
            return;
        }

        int mem_length = arraysize(stack_buf);
        while (true)
        {
            if (result < 0)
            {
                DLOG(WARNING) << "Unable to printf the requested string due to error.";
                return;
            }
            else
            {
                mem_length = result + 1;
            }

            if (mem_length > 32 * 1024 * 1024)
            {
                DLOG(WARNING) << "Unable to printf the requested string due to size.";
                return;
            }

            std::vector<typename StringType::value_type> mem_buf(mem_length);

            GG_VA_COPY(ap_copy, ap);
            result = vsnprintfT(&mem_buf[0], mem_length, format, ap_copy);
            va_end(ap_copy);

            if ((result >= 0) && (result < mem_length))
            {
                dst->append(&mem_buf[0], result);
                return;
            }
        }
    }

    std::string StringPrintf(const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        std::string result;
        StringAppendV(&result, format, ap);
        va_end(ap);
        return result;
    }

    std::wstring StringPrintf(const wchar_t* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        std::wstring result;
        StringAppendV(&result, format, ap);
        va_end(ap);
        return result;
    }

    std::string StringPrintV(const char* format, va_list ap)
    {
        std::string result;
        StringAppendV(&result, format, ap);
        return result;
    }

    const std::string& SStringPrintf(std::string* dst, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        dst->clear();
        StringAppendV(dst, format, ap);
        va_end(ap);
        return *dst;
    }

    const std::wstring& SStringPrintf(std::wstring* dst,
        const wchar_t* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        dst->clear();
        StringAppendV(dst, format, ap);
        va_end(ap);
        return *dst;
    }

    void StringAppendF(std::string* dst, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        StringAppendV(dst, format, ap);
        va_end(ap);
    }

    void StringAppendF(std::wstring* dst, const wchar_t* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        StringAppendV(dst, format, ap);
        va_end(ap);
    }

    void StringAppendV(std::string* dst, const char* format, va_list ap)
    {
        StringAppendVT(dst, format, ap);
    }

    void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap)
    {
        StringAppendVT(dst, format, ap);
    }

} //namespace base

