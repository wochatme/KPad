#ifndef __ui_base_animation_container_h__
#define __ui_base_animation_container_h__

#include <set>

#include "base/timer.h"

namespace ui
{
    class AnimationContainerElement;
    class AnimationContainerObserver;

    class AnimationContainer : public base::RefCounted<AnimationContainer>
    {
    public:
        AnimationContainer();

        void Start(AnimationContainerElement* animation);

        void Stop(AnimationContainerElement* animation);

        void set_observer(AnimationContainerObserver* observer)
        {
            observer_ = observer;
        }

        base::TimeTicks last_tick_time() const { return last_tick_time_; }

        bool is_running() const { return !elements_.empty(); }

    private:
        friend class base::RefCounted<AnimationContainer>;

        typedef std::set<AnimationContainerElement*> Elements;

        ~AnimationContainer();

        void Run();

        void SetMinTimerInterval(base::TimeDelta delta);

        base::TimeDelta GetMinInterval();

        base::TimeTicks last_tick_time_;

        Elements elements_;

        base::TimeDelta min_timer_interval_;

        base::RepeatingTimer<AnimationContainer> timer_;

        AnimationContainerObserver* observer_;

        DISALLOW_COPY_AND_ASSIGN(AnimationContainer);
    };

} //namespace ui

#endif //__ui_base_animation_container_h__

