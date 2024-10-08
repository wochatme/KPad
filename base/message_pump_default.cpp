#include "message_pump_default.h"

#include "logging.h"

namespace base
{
    MessagePumpDefault::MessagePumpDefault()
        : keep_running_(true), event_(false, false) {}

    void MessagePumpDefault::Run(Delegate* delegate)
    {
        DCHECK(keep_running_) << "Quit must have been called outside of Run!";

        for (;;)
        {
            bool did_work = delegate->DoWork();
            if (!keep_running_)
            {
                break;
            }

            did_work |= delegate->DoDelayedWork(&delayed_work_time_);
            if (!keep_running_)
            {
                break;
            }

            if (did_work)
            {
                continue;
            }

            did_work = delegate->DoIdleWork();
            if (!keep_running_)
            {
                break;
            }

            if (did_work)
            {
                continue;
            }

            if (delayed_work_time_.is_null())
            {
                event_.Wait();
            }
            else
            {
                TimeDelta delay = delayed_work_time_ - TimeTicks::Now();
                if (delay > TimeDelta())
                {
                    event_.TimedWait(delay);
                }
                else
                {
                    delayed_work_time_ = TimeTicks();
                }
            }
        }

        keep_running_ = true;
    }

    void MessagePumpDefault::Quit()
    {
        keep_running_ = false;
    }

    void MessagePumpDefault::ScheduleWork()
    {
        event_.Signal();
    }

    void MessagePumpDefault::ScheduleDelayedWork(const TimeTicks& delayed_work_time)
    {
        delayed_work_time_ = delayed_work_time;
    }

} //namespace base