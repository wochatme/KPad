#ifndef __ui_base_multi_animation_h__
#define __ui_base_multi_animation_h__

#include <vector>

#include "animation.h"
#include "tween.h"

namespace ui
{
    class MultiAnimation : public Animation
    {
    public:
        struct Part
        {
            Part() : time_ms(0), start_time_ms(0),
                end_time_ms(0), type(Tween::ZERO) {}
            Part(int time_ms, Tween::Type type) : time_ms(time_ms),
                start_time_ms(0), end_time_ms(time_ms), type(type) {}

            int time_ms;
            int start_time_ms;
            int end_time_ms;
            Tween::Type type;
        };

        typedef std::vector<Part> Parts;

        explicit MultiAnimation(const Parts& parts);
        virtual ~MultiAnimation();

        void set_continuous(bool continuous) { continuous_ = continuous; }

        virtual double GetCurrentValue() const { return current_value_; }

        size_t current_part_index() const { return current_part_index_; }

    protected:
        virtual void Step(base::TimeTicks time_now);
        virtual void SetStartTime(base::TimeTicks start_time);

    private:
        const Part& GetPart(int* time_ms, size_t* part_index);

        const Parts parts_;

        const int cycle_time_ms_;

        double current_value_;

        size_t current_part_index_;

        bool continuous_;

        DISALLOW_COPY_AND_ASSIGN(MultiAnimation);
    };

} //namespace ui

#endif //__ui_base_multi_animation_h__

