#ifndef __base_message_pump_h__
#define __base_message_pump_h__

#include "memory/ref_counted.h"

namespace base
{
    class TimeTicks;

    class MessagePump : public RefCountedThreadSafe<MessagePump>
    {
    public:
        class Delegate
        {
        public:
            virtual ~Delegate() {}

            virtual bool DoWork() = 0;

            virtual bool DoDelayedWork(TimeTicks* next_delayed_work_time) = 0;

            virtual bool DoIdleWork() = 0;
        };

        MessagePump();
        virtual ~MessagePump();

        virtual void Run(Delegate* delegate) = 0;

        virtual void Quit() = 0;

        virtual void ScheduleWork() = 0;

        virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) = 0;
    };

} //namespace base

#endif //__base_message_pump_h__