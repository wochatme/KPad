#ifndef __base_waitable_event_h__
#define __base_waitable_event_h__

#include <windows.h>
#include "base/basic_types.h"

namespace base
{
    static const int kNoTimeout = -1;

    class TimeDelta;

    class WaitableEvent
    {
    public:
        WaitableEvent(bool manual_reset, bool initially_signaled);

        explicit WaitableEvent(HANDLE event_handle);

        HANDLE Release();

        ~WaitableEvent();

        void Reset();

        void Signal();

        bool IsSignaled();

        bool Wait();

        bool TimedWait(const TimeDelta& max_time);

        HANDLE handle() const { return handle_; }

        static size_t WaitMany(WaitableEvent** waitables, size_t count);

        // This is a private helper class. It's here because it's used by friends of
        // this class (such as WaitableEventWatcher) to be able to enqueue elements
        // of the wait-list
        class Waiter
        {
        public:
            // Signal the waiter to wake up.
            //
            // Consider the case of a Waiter which is in multiple WaitableEvent's
            // wait-lists. Each WaitableEvent is automatic-reset and two of them are
            // signaled at the same time. Now, each will wake only the first waiter in
            // the wake-list before resetting. However, if those two waiters happen to
            // be the same object (as can happen if another thread didn't have a chance
            // to dequeue the waiter from the other wait-list in time), two auto-resets
            // will have happened, but only one waiter has been signaled!
            //
            // Because of this, a Waiter may "reject" a wake by returning false. In
            // this case, the auto-reset WaitableEvent shouldn't act as if anything has
            // been notified.
            virtual bool Fire(WaitableEvent* signaling_event) = 0;

            // Waiters may implement this in order to provide an extra condition for
            // two Waiters to be considered equal. In WaitableEvent::Dequeue, if the
            // pointers match then this function is called as a final check. See the
            // comments in ~Handle for why.
            virtual bool Compare(void* tag) = 0;

        protected:
            virtual ~Waiter() {}
        };

    private:
        friend class WaitableEventWatcher;

        HANDLE handle_;

        DISALLOW_COPY_AND_ASSIGN(WaitableEvent);
    };

} //namespace base

#endif //__base_waitable_event_h__