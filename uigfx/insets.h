#ifndef __ui_gfx_insets_h__
#define __ui_gfx_insets_h__

#include <string>

namespace gfx
{
    class Insets
    {
    public:
        Insets() : top_(0), left_(0), bottom_(0), right_(0) {}
        Insets(int top, int left, int bottom, int right)
            : top_(top), left_(left), bottom_(bottom), right_(right) {}
        ~Insets() {}

        int top() const { return top_; }
        int left() const { return left_; }
        int bottom() const { return bottom_; }
        int right() const { return right_; }

        int width() const { return left_ + right_; }

        int height() const { return top_ + bottom_; }

        bool empty() const { return width() == 0 && height() == 0; }

        void Set(int top, int left, int bottom, int right)
        {
            top_ = top;
            left_ = left;
            bottom_ = bottom;
            right_ = right;
        }

        bool operator==(const Insets& insets) const
        {
            return top_ == insets.top_ && left_ == insets.left_ &&
                bottom_ == insets.bottom_ && right_ == insets.right_;
        }

        bool operator!=(const Insets& insets) const
        {
            return !(*this == insets);
        }

        Insets& operator+=(const Insets& insets)
        {
            top_ += insets.top_;
            left_ += insets.left_;
            bottom_ += insets.bottom_;
            right_ += insets.right_;
            return *this;
        }

        std::string ToString() const;

    private:
        int top_;
        int left_;
        int bottom_;
        int right_;
    };

} //namespace gfx

#endif //__ui_gfx_insets_h__

