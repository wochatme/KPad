#ifndef __base_time_h__
#define __base_time_h__

#include <time.h>
#include <windows.h>

#include "basic_types.h"

namespace base
{

    class Time;
    class TimeTicks;

    class TimeDelta
    {
    public:
        TimeDelta() : delta_(0) {}

        static TimeDelta FromDays(int64 days);
        static TimeDelta FromHours(int64 hours);
        static TimeDelta FromMinutes(int64 minutes);
        static TimeDelta FromSeconds(int64 secs);
        static TimeDelta FromMilliseconds(int64 ms);
        static TimeDelta FromMicroseconds(int64 us);

        // Converts an integer value representing TimeDelta to a class. This is used
        // when deserializing a |TimeDelta| structure, using a value known to be
        // compatible. It is not provided as a constructor because the integer type
        // may be unclear from the perspective of a caller.
        static TimeDelta FromInternalValue(int64 delta)
        {
            return TimeDelta(delta);
        }

        // For serializing, use FromInternalValue to reconstitute.
        int64 ToInternalValue() const
        {
            return delta_;
        }

        int InDays() const;
        int InHours() const;
        int InMinutes() const;
        double InSecondsF() const;
        int64 InSeconds() const;
        double InMillisecondsF() const;
        int64 InMilliseconds() const;
        int64 InMillisecondsRoundedUp() const;
        int64 InMicroseconds() const;

        TimeDelta& operator=(TimeDelta other)
        {
            delta_ = other.delta_;
            return *this;
        }

        TimeDelta operator+(TimeDelta other) const
        {
            return TimeDelta(delta_ + other.delta_);
        }
        TimeDelta operator-(TimeDelta other) const
        {
            return TimeDelta(delta_ - other.delta_);
        }

        TimeDelta& operator+=(TimeDelta other)
        {
            delta_ += other.delta_;
            return *this;
        }
        TimeDelta& operator-=(TimeDelta other)
        {
            delta_ -= other.delta_;
            return *this;
        }
        TimeDelta operator-() const
        {
            return TimeDelta(-delta_);
        }

        TimeDelta operator*(int64 a) const
        {
            return TimeDelta(delta_ * a);
        }
        TimeDelta operator/(int64 a) const
        {
            return TimeDelta(delta_ / a);
        }
        TimeDelta& operator*=(int64 a)
        {
            delta_ *= a;
            return *this;
        }
        TimeDelta& operator/=(int64 a)
        {
            delta_ /= a;
            return *this;
        }
        int64 operator/(TimeDelta a) const
        {
            return delta_ / a.delta_;
        }

        Time operator+(Time t) const;
        TimeTicks operator+(TimeTicks t) const;

        bool operator==(TimeDelta other) const
        {
            return delta_ == other.delta_;
        }
        bool operator!=(TimeDelta other) const
        {
            return delta_ != other.delta_;
        }
        bool operator<(TimeDelta other) const
        {
            return delta_ < other.delta_;
        }
        bool operator<=(TimeDelta other) const
        {
            return delta_ <= other.delta_;
        }
        bool operator>(TimeDelta other) const
        {
            return delta_ > other.delta_;
        }
        bool operator>=(TimeDelta other) const
        {
            return delta_ >= other.delta_;
        }

    private:
        friend class Time;
        friend class TimeTicks;
        friend TimeDelta operator*(int64 a, TimeDelta td);

        explicit TimeDelta(int64 delta_us) : delta_(delta_us) {}

        int64 delta_;
    };

    class Time
    {
    public:
        static const int64 kMillisecondsPerSecond = 1000;
        static const int64 kMicrosecondsPerMillisecond = 1000;
        static const int64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
            kMillisecondsPerSecond;
        static const int64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
        static const int64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
        static const int64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
        static const int64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
        static const int64 kNanosecondsPerMicrosecond = 1000;
        static const int64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
            kMicrosecondsPerSecond;

        struct Exploded
        {
            int year;          // 
            int month;         // 
            int day_of_week;   // 
            int day_of_month;  // 
            int hour;          // 
            int minute;        // 
            int second;        // 
            int millisecond;   // 

            bool HasValidValues() const;
        };

        explicit Time() : us_(0) {}

        bool is_null() const
        {
            return us_ == 0;
        }

        static Time UnixEpoch();

        static Time Now();

        static Time NowFromSystemTime();

        static Time FromTimeT(time_t tt);
        time_t ToTimeT() const;

        static Time FromDoubleT(double dt);
        double ToDoubleT() const;

        static Time FromFileTime(FILETIME ft);
        FILETIME ToFileTime() const;

        static const int kMinLowResolutionThresholdMs = 16;

        static void EnableHighResolutionTimer(bool enable);

        static bool ActivateHighResolutionTimer(bool activate);

        // Returns true if the high resolution timer is both enabled and activated.
        // This is provided for testing only, and is not tracked in a thread-safe
        // way.
        static bool IsHighResolutionTimerInUse();

        static Time FromUTCExploded(const Exploded& exploded)
        {
            return FromExploded(false, exploded);
        }
        static Time FromLocalExploded(const Exploded& exploded)
        {
            return FromExploded(true, exploded);
        }

        static Time FromInternalValue(int64 us)
        {
            return Time(us);
        }

        static bool FromString(const char* time_string, Time* parsed_time);

        int64 ToInternalValue() const
        {
            return us_;
        }

        void UTCExplode(Exploded* exploded) const
        {
            return Explode(false, exploded);
        }
        void LocalExplode(Exploded* exploded) const
        {
            return Explode(true, exploded);
        }

        Time LocalMidnight() const;

        Time& operator=(Time other)
        {
            us_ = other.us_;
            return *this;
        }

        TimeDelta operator-(Time other) const
        {
            return TimeDelta(us_ - other.us_);
        }

        Time& operator+=(TimeDelta delta)
        {
            us_ += delta.delta_;
            return *this;
        }
        Time& operator-=(TimeDelta delta)
        {
            us_ -= delta.delta_;
            return *this;
        }

        Time operator+(TimeDelta delta) const
        {
            return Time(us_ + delta.delta_);
        }
        Time operator-(TimeDelta delta) const
        {
            return Time(us_ - delta.delta_);
        }

        bool operator==(Time other) const
        {
            return us_ == other.us_;
        }
        bool operator!=(Time other) const
        {
            return us_ != other.us_;
        }
        bool operator<(Time other) const
        {
            return us_ < other.us_;
        }
        bool operator<=(Time other) const
        {
            return us_ <= other.us_;
        }
        bool operator>(Time other) const
        {
            return us_ > other.us_;
        }
        bool operator>=(Time other) const
        {
            return us_ >= other.us_;
        }

    private:
        friend class TimeDelta;

        explicit Time(int64 us) : us_(us) {}

        void Explode(bool is_local, Exploded* exploded) const;

        static Time FromExploded(bool is_local, const Exploded& exploded);

        static const int64 kTimeTToMicrosecondsOffset;

        static bool high_resolution_timer_enabled_;

        // Count of activations on the high resolution timer.  Only use in tests
        // which are single threaded.
        static int high_resolution_timer_activated_;

        int64 us_;
    };

    // static
    inline TimeDelta TimeDelta::FromDays(int64 days)
    {
        return TimeDelta(days * Time::kMicrosecondsPerDay);
    }

    // static
    inline TimeDelta TimeDelta::FromHours(int64 hours)
    {
        return TimeDelta(hours * Time::kMicrosecondsPerHour);
    }

    // static
    inline TimeDelta TimeDelta::FromMinutes(int64 minutes)
    {
        return TimeDelta(minutes * Time::kMicrosecondsPerMinute);
    }

    // static
    inline TimeDelta TimeDelta::FromSeconds(int64 secs)
    {
        return TimeDelta(secs * Time::kMicrosecondsPerSecond);
    }

    // static
    inline TimeDelta TimeDelta::FromMilliseconds(int64 ms)
    {
        return TimeDelta(ms * Time::kMicrosecondsPerMillisecond);
    }

    // static
    inline TimeDelta TimeDelta::FromMicroseconds(int64 us)
    {
        return TimeDelta(us);
    }

    inline Time TimeDelta::operator+(Time t) const
    {
        return Time(t.us_ + delta_);
    }

    // TimeTicks ------------------------------------------------------------------

    class TimeTicks
    {
    public:
        TimeTicks() : ticks_(0) {}

        static TimeTicks Now();

        static TimeTicks HighResNow();

        static int64 GetQPCDriftMicroseconds();

        static bool IsHighResClockWorking();

        bool is_null() const
        {
            return ticks_ == 0;
        }

        // Converts an integer value representing TimeTicks to a class. This is used
        // when deserializing a |TimeTicks| structure, using a value known to be
        // compatible. It is not provided as a constructor because the integer type
        // may be unclear from the perspective of a caller.
        static TimeTicks FromInternalValue(int64 ticks)
        {
            return TimeTicks(ticks);
        }

        // For serializing, use FromInternalValue to reconstitute.
        int64 ToInternalValue() const
        {
            return ticks_;
        }

        TimeTicks& operator=(TimeTicks other)
        {
            ticks_ = other.ticks_;
            return *this;
        }

        TimeDelta operator-(TimeTicks other) const
        {
            return TimeDelta(ticks_ - other.ticks_);
        }

        TimeTicks& operator+=(TimeDelta delta)
        {
            ticks_ += delta.delta_;
            return *this;
        }
        TimeTicks& operator-=(TimeDelta delta)
        {
            ticks_ -= delta.delta_;
            return *this;
        }

        TimeTicks operator+(TimeDelta delta) const
        {
            return TimeTicks(ticks_ + delta.delta_);
        }
        TimeTicks operator-(TimeDelta delta) const
        {
            return TimeTicks(ticks_ - delta.delta_);
        }

        bool operator==(TimeTicks other) const
        {
            return ticks_ == other.ticks_;
        }
        bool operator!=(TimeTicks other) const
        {
            return ticks_ != other.ticks_;
        }
        bool operator<(TimeTicks other) const
        {
            return ticks_ < other.ticks_;
        }
        bool operator<=(TimeTicks other) const
        {
            return ticks_ <= other.ticks_;
        }
        bool operator>(TimeTicks other) const
        {
            return ticks_ > other.ticks_;
        }
        bool operator>=(TimeTicks other) const
        {
            return ticks_ >= other.ticks_;
        }

    protected:
        friend class TimeDelta;

        explicit TimeTicks(int64 ticks) : ticks_(ticks) {}

        int64 ticks_;

        typedef DWORD(*TickFunctionType)(void);
        static TickFunctionType SetMockTickFunction(TickFunctionType ticker);
    };

    inline TimeTicks TimeDelta::operator+(TimeTicks t) const
    {
        return TimeTicks(t.ticks_ + delta_);
    }

} //namespace base

#endif //__base_time_h__