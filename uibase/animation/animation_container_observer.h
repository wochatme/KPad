#ifndef __ui_base_animation_container_observer_h__
#define __ui_base_animation_container_observer_h__

namespace ui
{
    class AnimationContainer;

    class AnimationContainerObserver
    {
    public:
        virtual void AnimationContainerProgressed(
            AnimationContainer* container) = 0;

        virtual void AnimationContainerEmpty(AnimationContainer* container) = 0;

    protected:
        virtual ~AnimationContainerObserver() {}
    };

} //namespace ui

#endif //__ui_base_animation_container_observer_h__

