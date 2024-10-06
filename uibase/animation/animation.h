#ifndef __ui_base_animation_h__
#define __ui_base_animation_h__

#include "base/memory/ref_counted.h"

#include "animation_container_element.h"

namespace gfx
{
    class Rect;
}

namespace ui
{

    class AnimationContainer;
    class AnimationDelegate;

    class Animation : public AnimationContainerElement
    {
    public:
        explicit Animation(base::TimeDelta timer_interval);
        virtual ~Animation();

        void Start();

        void Stop();

        virtual double GetCurrentValue() const = 0;

        double CurrentValueBetween(double start, double target) const;
        int CurrentValueBetween(int start, int target) const;
        gfx::Rect CurrentValueBetween(const gfx::Rect& start_bounds,
            const gfx::Rect& target_bounds) const;

        void set_delegate(AnimationDelegate* delegate) { delegate_ = delegate; }

        void SetContainer(AnimationContainer* container);

        bool is_animating() const { return is_animating_; }

        base::TimeDelta timer_interval() const { return timer_interval_; }

        static bool ShouldRenderRichAnimation();

    protected:
        virtual void AnimationStarted() {}

        virtual void AnimationStopped() {}

        virtual bool ShouldSendCanceledFromStop() { return false; }

        AnimationContainer* container() { return container_.get(); }
        base::TimeTicks start_time() const { return start_time_; }
        AnimationDelegate* delegate() { return delegate_; }

        virtual void SetStartTime(base::TimeTicks start_time);
        virtual void Step(base::TimeTicks time_now) = 0;
        virtual base::TimeDelta GetTimerInterval() const { return timer_interval_; }

    private:
        const base::TimeDelta timer_interval_;

        bool is_animating_;

        AnimationDelegate* delegate_;

        scoped_refptr<AnimationContainer> container_;

        base::TimeTicks start_time_;

        DISALLOW_COPY_AND_ASSIGN(Animation);
    };

} //namespace ui

#endif //__ui_base_animation_h__
