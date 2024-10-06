#ifndef __ui_base_slide_animation_h__
#define __ui_base_slide_animation_h__

#include "linear_animation.h"
#include "tween.h"

namespace ui
{

    // Slide Animation
    //
    //
    //     #include "app/slide_animation.h"
    //
    //     class MyClass : public AnimationDelegate {
    //     public:
    //       MyClass() {
    //         animation_.reset(new SlideAnimation(this));
    //         animation_->SetSlideDuration(500);
    //       }
    //       void OnMouseOver() {
    //         animation_->Show();
    //       }
    //       void OnMouseOut() {
    //         animation_->Hide();
    //       }
    //       void AnimationProgressed(const Animation* animation) {
    //         if(animation == animation_.get()) {
    //           Layout();
    //           SchedulePaint();
    //         } else if(animation == other_animation_.get()) {
    //           ...
    //         }
    //       }
    //       void Layout() {
    //         if(animation_->is_animating()) {
    //           hover_image_.SetOpacity(animation_->GetCurrentValue());
    //         }
    //       }
    //     private:
    //       scoped_ptr<SlideAnimation> animation_;
    //     }
    class SlideAnimation : public LinearAnimation
    {
    public:
        explicit SlideAnimation(AnimationDelegate* target);
        virtual ~SlideAnimation();

        virtual void Reset();
        virtual void Reset(double value);

        virtual void Show();

        virtual void Hide();

        virtual void SetSlideDuration(int duration) { slide_duration_ = duration; }
        int GetSlideDuration() const { return slide_duration_; }
        void SetTweenType(Tween::Type tween_type) { tween_type_ = tween_type; }

        double GetCurrentValue() const { return value_current_; }
        bool IsShowing() const { return showing_; }
        bool IsClosing() const { return !showing_ && value_end_ < value_current_; }

    private:
        void AnimateToState(double state);

        AnimationDelegate* target_;

        Tween::Type tween_type_;

        bool showing_;

        double value_start_;
        double value_end_;
        double value_current_;

        int slide_duration_;

        DISALLOW_COPY_AND_ASSIGN(SlideAnimation);
    };

} //namespace ui

#endif //__ui_base_slide_animation_h__

