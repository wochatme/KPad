#ifndef __ui_base_animation_delegate_h__
#define __ui_base_animation_delegate_h__

#pragma once

namespace ui
{
    class Animation;

    // AnimationDelegate
    //
    class AnimationDelegate
    {
    public:
        virtual void AnimationEnded(const Animation* animation) {}

        virtual void AnimationProgressed(const Animation* animation) {}

        virtual void AnimationCanceled(const Animation* animation) {}

    protected:
        virtual ~AnimationDelegate() {}
    };

} //namespace ui

#endif //__ui_base_animation_delegate_h__

