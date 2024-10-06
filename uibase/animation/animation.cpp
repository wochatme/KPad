#if 0
#include "animation.h"
#include "base/win/windows_version.h"

#include "uigfx/rect.h"

#include "animation_container.h"
#include "animation_delegate.h"
#include "tween.h"

namespace ui
{

    Animation::Animation(base::TimeDelta timer_interval)
        : timer_interval_(timer_interval),
        is_animating_(false), delegate_(NULL) {}

    Animation::~Animation()
    {
        if (is_animating_)
        {
            container_->Stop(this);
        }
    }

    void Animation::Start()
    {
        if (is_animating_)
        {
            return;
        }

        if (!container_.get())
        {
            container_ = new AnimationContainer();
        }

        is_animating_ = true;

        container_->Start(this);

        AnimationStarted();
    }

    void Animation::Stop()
    {
        if (!is_animating_)
        {
            return;
        }

        is_animating_ = false;

        container_->Stop(this);

        AnimationStopped();

        if (delegate_)
        {
            if (ShouldSendCanceledFromStop())
            {
                delegate_->AnimationCanceled(this);
            }
            else
            {
                delegate_->AnimationEnded(this);
            }
        }
    }

    double Animation::CurrentValueBetween(double start, double target) const
    {
        return Tween::ValueBetween(GetCurrentValue(), start, target);
    }

    int Animation::CurrentValueBetween(int start, int target) const
    {
        return Tween::ValueBetween(GetCurrentValue(), start, target);

    }

    gfx::Rect Animation::CurrentValueBetween(const gfx::Rect& start_bounds,
        const gfx::Rect& target_bounds) const
    {
        return Tween::ValueBetween(GetCurrentValue(), start_bounds, target_bounds);
    }

    void Animation::SetContainer(AnimationContainer* container)
    {
        if (container == container_.get())
        {
            return;
        }

        if (is_animating_)
        {
            container_->Stop(this);
        }

        if (container)
        {
            container_ = container;
        }
        else
        {
            container_ = new AnimationContainer();
        }

        if (is_animating_)
        {
            container_->Start(this);
        }
    }

    // static
    bool Animation::ShouldRenderRichAnimation()
    {
        if (base::win::GetVersion() >= base::win::VERSION_VISTA)
        {
            BOOL result;

            if (SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, 0, &result, 0))
            {
                //   http://msdn.microsoft.com/en-us/library/ms724947(VS.85).aspx
                return !!result;
            }
        }
        return !::GetSystemMetrics(SM_REMOTESESSION);
    }

    void Animation::SetStartTime(base::TimeTicks start_time)
    {
        start_time_ = start_time;
    }

} //namespace ui
#endif 
