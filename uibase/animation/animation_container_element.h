#ifndef __ui_base_animation_container_element_h__
#define __ui_base_animation_container_element_h__

#include "base/time.h"

namespace ui
{
    class AnimationContainerElement
    {
    public:
        virtual void SetStartTime(base::TimeTicks start_time) = 0;

        virtual void Step(base::TimeTicks time_now) = 0;

        virtual base::TimeDelta GetTimerInterval() const = 0;

    protected:
        virtual ~AnimationContainerElement() {}
    };

} //namespace ui

#endif //__ui_base_animation_container_element_h__

