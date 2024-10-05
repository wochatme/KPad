#ifndef __base_stack_trace_h__
#define __base_stack_trace_h__

#include <iosfwd>

struct _EXCEPTION_POINTERS;

namespace base
{
    namespace debug
    {
        class StackTrace
        {
        public:
            StackTrace();

            StackTrace(_EXCEPTION_POINTERS* exception_pointers);

            // Creates a stacktrace from an existing array of instruction
            // pointers (such as returned by Addresses()).  |count| will be
            // trimmed to |kMaxTraces|.
            StackTrace(const void* const* trace, size_t count);

            // Copying and assignment are allowed with the default functions.
            ~StackTrace();

            const void* const* Addresses(size_t* count);
            void PrintBacktrace();
            void OutputToStream(std::ostream* os);

        private:
            // http://msdn.microsoft.com/en-us/library/bb204633.aspx,
            static const int MAX_TRACES = 62;
            void* trace_[MAX_TRACES];
            int count_;
        };

    } //namespace debug
} //namespace base

#endif //__base_stack_trace_h__