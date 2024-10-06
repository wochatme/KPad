#if 0
#include "base_time.h"

#include "third_party/nspr/prtime.h"

#include "cpu.h"
#include "memory/singleton.h"
#include "synchronization/lock.h"
#include "sys_string_conversions.h"
#include "threading/platform_thread.h"

#pragma comment(lib, "winmm.lib")

namespace base
{

    int TimeDelta::InDays() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
    }

    int TimeDelta::InHours() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
    }

    int TimeDelta::InMinutes() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
    }

    double TimeDelta::InSecondsF() const
    {
        return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
    }

    int64 TimeDelta::InSeconds() const
    {
        return delta_ / Time::kMicrosecondsPerSecond;
    }

    double TimeDelta::InMillisecondsF() const
    {
        return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMilliseconds() const
    {
        return delta_ / Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMillisecondsRoundedUp() const
    {
        return (delta_ + Time::kMicrosecondsPerMillisecond - 1) /
            Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMicroseconds() const
    {
        return delta_;
    }

    // static
    Time Time::FromTimeT(time_t tt)
    {
        if (tt == 0)
        {
            return Time(); 
        }
        return Time((tt * kMicrosecondsPerSecond) + kTimeTToMicrosecondsOffset);
    }

    time_t Time::ToTimeT() const
    {
        if (us_ == 0)
        {
            return 0; 
        }
        return (us_ - kTimeTToMicrosecondsOffset) / kMicrosecondsPerSecond;
    }

    // static
    Time Time::FromDoubleT(double dt)
    {
        if (dt == 0)
        {
            return Time(); 
        }
        return Time(static_cast<int64>((dt *
            static_cast<double>(kMicrosecondsPerSecond)) +
            kTimeTToMicrosecondsOffset));
    }

    double Time::ToDoubleT() const
    {
        if (us_ == 0)
        {
            return 0; 
        }
        return (static_cast<double>(us_ - kTimeTToMicrosecondsOffset) /
            static_cast<double>(kMicrosecondsPerSecond));
    }

    // static
    Time Time::UnixEpoch()
    {
        Time time;
        time.us_ = kTimeTToMicrosecondsOffset;
        return time;
    }

    Time Time::LocalMidnight() const
    {
        Exploded exploded;
        LocalExplode(&exploded);
        exploded.hour = 0;
        exploded.minute = 0;
        exploded.second = 0;
        exploded.millisecond = 0;
        return FromLocalExploded(exploded);
    }

    // static
    bool Time::FromString(const char* time_string, Time* parsed_time)
    {
        DCHECK((time_string != NULL) && (parsed_time != NULL));

        if (time_string[0] == '\0')
        {
            return false;
        }

        PRTime result_time = 0;
        PRStatus result = PR_ParseTimeString(time_string, PR_FALSE,
            &result_time);
        if (PR_SUCCESS != result)
        {
            return false;
        }

        result_time += kTimeTToMicrosecondsOffset;
        *parsed_time = Time(result_time);
        return true;
    }

    inline bool is_in_range(int value, int lo, int hi)
    {
        return lo <= value && value <= hi;
    }

    bool Time::Exploded::HasValidValues() const
    {
        return is_in_range(month, 1, 12) &&
            is_in_range(day_of_week, 0, 6) &&
            is_in_range(day_of_month, 1, 31) &&
            is_in_range(hour, 0, 23) &&
            is_in_range(minute, 0, 59) &&
            is_in_range(second, 0, 60) &&
            is_in_range(millisecond, 0, 999);
    }

} //namespace base


//  http://www.ddj.com/windows/184416651
//  http://bugzilla.mozilla.org/show_bug.cgi?id=363258
//
// http://softwarecommunity.intel.com/articles/eng/1086.htm
//

using base::Time;
using base::TimeDelta;
using base::TimeTicks;

namespace
{

    int64 FileTimeToMicroseconds(const FILETIME& ft)
    {
        return bit_cast<int64, FILETIME>(ft) / 10;
    }

    void MicrosecondsToFileTime(int64 us, FILETIME* ft)
    {
        DCHECK_GE(us, 0LL) << "Time is less than 0, negative values are not "
            "representable in FILETIME";

        *ft = bit_cast<FILETIME, int64>(us * 10);
    }

    int64 CurrentWallclockMicroseconds()
    {
        FILETIME ft;
        ::GetSystemTimeAsFileTime(&ft);
        return FileTimeToMicroseconds(ft);
    }

    const int kMaxMillisecondsToAvoidDrift = 60 * Time::kMillisecondsPerSecond;

    int64 initial_time = 0;
    TimeTicks initial_ticks;

    void InitializeClock()
    {
        initial_ticks = TimeTicks::Now();
        initial_time = CurrentWallclockMicroseconds();
    }

} //namespace

// static
const int64 Time::kTimeTToMicrosecondsOffset = GG_INT64_C(11644473600000000);

bool Time::high_resolution_timer_enabled_ = false;
int Time::high_resolution_timer_activated_ = 0;

// static
Time Time::Now()
{
    if (initial_time == 0)
    {
        InitializeClock();
    }

    while (true)
    {
        TimeTicks ticks = TimeTicks::Now();

        TimeDelta elapsed = ticks - initial_ticks;

        if (elapsed.InMilliseconds() > kMaxMillisecondsToAvoidDrift)
        {
            InitializeClock();
            continue;
        }

        return Time(elapsed + Time(initial_time));
    }
}

// static
Time Time::NowFromSystemTime()
{
    InitializeClock();
    return Time(initial_time);
}

// static
Time Time::FromFileTime(FILETIME ft)
{
    return Time(FileTimeToMicroseconds(ft));
}

FILETIME Time::ToFileTime() const
{
    FILETIME utc_ft;
    MicrosecondsToFileTime(us_, &utc_ft);
    return utc_ft;
}

// static
void Time::EnableHighResolutionTimer(bool enable)
{
    static base::PlatformThreadId my_thread = base::PlatformThread::CurrentId();
    DCHECK(base::PlatformThread::CurrentId() == my_thread);

    if (high_resolution_timer_enabled_ == enable)
    {
        return;
    }

    high_resolution_timer_enabled_ = enable;
}

// static
bool Time::ActivateHighResolutionTimer(bool activate)
{
    if (!high_resolution_timer_enabled_)
    {
        return false;
    }

    const int kMinTimerIntervalMs = 1;
    MMRESULT result;
    if (activate)
    {
        result = timeBeginPeriod(kMinTimerIntervalMs);
        high_resolution_timer_activated_++;
    }
    else
    {
        result = timeEndPeriod(kMinTimerIntervalMs);
        high_resolution_timer_activated_--;
    }
    return result == TIMERR_NOERROR;
}

// static
bool Time::IsHighResolutionTimerInUse()
{
    // Note:  we should track the high_resolution_timer_activated_ value
    // under a lock if we want it to be accurate in a system with multiple
    // message loops.  We don't do that - because we don't want to take the
    // expense of a lock for this.  We *only* track this value so that unit
    // tests can see if the high resolution timer is on or off.
    return high_resolution_timer_enabled_ &&
        high_resolution_timer_activated_ > 0;
}

// static
Time Time::FromExploded(bool is_local, const Exploded& exploded)
{
    SYSTEMTIME st;
    st.wYear = exploded.year;
    st.wMonth = exploded.month;
    st.wDayOfWeek = exploded.day_of_week;
    st.wDay = exploded.day_of_month;
    st.wHour = exploded.hour;
    st.wMinute = exploded.minute;
    st.wSecond = exploded.second;
    st.wMilliseconds = exploded.millisecond;

    FILETIME ft;
    bool success = true;

    if (is_local)
    {
        SYSTEMTIME utc_st;
        success = TzSpecificLocalTimeToSystemTime(NULL, &st, &utc_st) &&
            SystemTimeToFileTime(&utc_st, &ft);
    }
    else
    {
        success = !!SystemTimeToFileTime(&st, &ft);
    }

    if (!success)
    {
        NOTREACHED() << "Unable to convert time";
        return Time(0);
    }
    return Time(FileTimeToMicroseconds(ft));
}

void Time::Explode(bool is_local, Exploded* exploded) const
{
    if (us_ < 0LL)
    {
        // We are not able to convert it to FILETIME.
        ZeroMemory(exploded, sizeof(*exploded));
        return;
    }

    // FILETIME in UTC.
    FILETIME utc_ft;
    MicrosecondsToFileTime(us_, &utc_ft);

    // FILETIME in local time if necessary.
    bool success = true;
    // FILETIME in SYSTEMTIME (exploded).
    SYSTEMTIME st;
    if (is_local)
    {
        SYSTEMTIME utc_st;
        // We don't use FileTimeToLocalFileTime here, since it uses the current
        // settings for the time zone and daylight saving time. Therefore, if it is
        // daylight saving time, it will take daylight saving time into account,
        // even if the time you are converting is in standard time.
        success = FileTimeToSystemTime(&utc_ft, &utc_st) &&
            SystemTimeToTzSpecificLocalTime(NULL, &utc_st, &st);
    }
    else
    {
        success = !!FileTimeToSystemTime(&utc_ft, &st);
    }

    if (!success)
    {
        NOTREACHED() << "Unable to convert time, don't know why";
        ZeroMemory(exploded, sizeof(*exploded));
        return;
    }

    exploded->year = st.wYear;
    exploded->month = st.wMonth;
    exploded->day_of_week = st.wDayOfWeek;
    exploded->day_of_month = st.wDay;
    exploded->hour = st.wHour;
    exploded->minute = st.wMinute;
    exploded->second = st.wSecond;
    exploded->millisecond = st.wMilliseconds;
}

namespace
{

    // We define a wrapper to adapt between the __stdcall and __cdecl call of the
    // mock function, and to avoid a static constructor.  Assigning an import to a
    // function pointer directly would require setup code to fetch from the IAT.
    DWORD timeGetTimeWrapper()
    {
        return timeGetTime();
    }

    DWORD(*tick_function)(void) = &timeGetTimeWrapper;


    int64 rollover_ms = 0;

    DWORD last_seen_now = 0;

    base::Lock rollover_lock;

    TimeDelta RolloverProtectedNow()
    {
        base::AutoLock locked(rollover_lock);

        DWORD now = tick_function();
        if (now < last_seen_now)
        {
            rollover_ms += 0x100000000I64; // ~49.7 days.
        }
        last_seen_now = now;
        return TimeDelta::FromMilliseconds(now + rollover_ms);
    }

    class HighResNowSingleton
    {
    public:
        static HighResNowSingleton* GetInstance()
        {
            return Singleton<HighResNowSingleton>::get();
        }

        bool IsUsingHighResClock()
        {
            return ticks_per_microsecond_ != 0.0;
        }

        void DisableHighResClock()
        {
            ticks_per_microsecond_ = 0.0;
        }

        TimeDelta Now()
        {
            if (IsUsingHighResClock())
            {
                return TimeDelta::FromMicroseconds(UnreliableNow());
            }

            return RolloverProtectedNow();
        }

        int64 GetQPCDriftMicroseconds()
        {
            if (!IsUsingHighResClock())
            {
                return 0;
            }

            return abs((UnreliableNow() - ReliableNow()) - skew_);
        }

    private:
        HighResNowSingleton() : ticks_per_microsecond_(0.0), skew_(0)
        {
            InitializeClock();

            base::CPU cpu;
            if (cpu.vendor_name() == "AuthenticAMD" && cpu.family() == 15)
            {
                DisableHighResClock();
            }
        }

        void InitializeClock()
        {
            LARGE_INTEGER ticks_per_sec = { 0 };
            if (!QueryPerformanceFrequency(&ticks_per_sec))
            {
                return; 
            }
            ticks_per_microsecond_ = static_cast<float>(ticks_per_sec.QuadPart) /
                static_cast<float>(Time::kMicrosecondsPerSecond);

            skew_ = UnreliableNow() - ReliableNow();
        }

        int64 UnreliableNow()
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return static_cast<int64>(now.QuadPart / ticks_per_microsecond_);
        }

        int64 ReliableNow()
        {
            return RolloverProtectedNow().InMicroseconds();
        }

        float ticks_per_microsecond_;  
        int64 skew_; 

        friend struct DefaultSingletonTraits<HighResNowSingleton>;
    };

}  //namespace

// static
TimeTicks::TickFunctionType TimeTicks::SetMockTickFunction(
    TickFunctionType ticker)
{
    TickFunctionType old = tick_function;
    tick_function = ticker;
    return old;
}

// static
TimeTicks TimeTicks::Now()
{
    return TimeTicks() + RolloverProtectedNow();
}

// static
TimeTicks TimeTicks::HighResNow()
{
    return TimeTicks() + HighResNowSingleton::GetInstance()->Now();
}

// static
int64 TimeTicks::GetQPCDriftMicroseconds()
{
    return HighResNowSingleton::GetInstance()->GetQPCDriftMicroseconds();
}

// static
bool TimeTicks::IsHighResClockWorking()
{
    return HighResNowSingleton::GetInstance()->IsUsingHighResClock();
}
#endif 