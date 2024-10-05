#ifndef __base_debugger_h__
#define __base_debugger_h__

namespace base
{
    namespace debug
    {
        bool SpawnDebuggerOnProcess(unsigned process_id);

        bool WaitForDebugger(int wait_seconds, bool silent);

        bool BeingDebugged();

        void BreakDebugger();

        void SetSuppressDebugUI(bool suppress);
        bool IsDebugUISuppressed();

    } //namespace debug
} //namespace base

#endif //__base_debugger_h__