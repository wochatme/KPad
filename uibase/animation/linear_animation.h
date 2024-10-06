#ifndef __ui_base_linear_animation_h__
#define __ui_base_linear_animation_h__

#include "animation.h"

namespace ui
{
    class AnimationDelegate;

    class LinearAnimation : public Animation
    {
    public:
        LinearAnimation(int frame_rate, AnimationDelegate* delegate);

        LinearAnimation(int duration, int frame_rate, AnimationDelegate* delegate);

        virtual double GetCurrentValue() const;

        void End();

        void SetDuration(int duration);

    protected:
        virtual void AnimateToState(double state) = 0;

        virtual void Step(base::TimeTicks time_now);

        virtual void AnimationStopped();

        virtual bool ShouldSendCanceledFromStop();

    private:
        base::TimeDelta duration_;

        double state_;

        bool in_end_;

        DISALLOW_COPY_AND_ASSIGN(LinearAnimation);
    };

} //namespace ui

#endif //__ui_base_linear_animation_h__
