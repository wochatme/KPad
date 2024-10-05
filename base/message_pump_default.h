#ifndef __base_message_pump_default_h__
#define __base_message_pump_default_h__

#include "message_pump.h"
#include "synchronization/waitable_event.h"
#include "base_time.h"

namespace base
{
    class MessagePumpDefault : public MessagePump
    {
    public:
        MessagePumpDefault();
        ~MessagePumpDefault() {}

        virtual void Run(Delegate* delegate);
        virtual void Quit();
        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time);

    private:
        bool keep_running_;

        WaitableEvent event_;

        TimeTicks delayed_work_time_;

        DISALLOW_COPY_AND_ASSIGN(MessagePumpDefault);
    };

} //namespace base

#endif //__base_message_pump_default_h__