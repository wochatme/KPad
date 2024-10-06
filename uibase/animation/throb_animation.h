#ifndef __ui_base_throb_animation_h__
#define __ui_base_throb_animation_h__

#include "slide_animation.h"

namespace ui
{
    class ThrobAnimation : public SlideAnimation
    {
    public:
        explicit ThrobAnimation(AnimationDelegate* target);
        virtual ~ThrobAnimation() {}

        void StartThrobbing(int cycles_til_stop);

        void SetThrobDuration(int duration) { throb_duration_ = duration; }

        virtual void Reset();
        virtual void Show();
        virtual void Hide();

        virtual void SetSlideDuration(int duration) { slide_duration_ = duration; }

        void set_cycles_remaining(int value) { cycles_remaining_ = value; }
        int cycles_remaining() const { return cycles_remaining_; }

    protected:
        virtual void Step(base::TimeTicks time_now);

    private:
        void ResetForSlide();

        int slide_duration_;

        int throb_duration_;

        int cycles_remaining_;

        bool throbbing_;

        DISALLOW_COPY_AND_ASSIGN(ThrobAnimation);
    };

} //namespace ui

#endif //__ui_base_throb_animation_h__

