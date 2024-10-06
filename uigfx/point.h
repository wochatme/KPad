#ifndef __ui_gfx_point_h__
#define __ui_gfx_point_h__

#include <iosfwd>

typedef unsigned long DWORD;
typedef struct tagPOINT POINT;

namespace gfx
{
    class Point
    {
    public:
        Point();
        Point(int x, int y);

        explicit Point(DWORD point);
        explicit Point(const POINT& point);
        Point& operator=(const POINT& point);

        ~Point() {}

        int x() const { return x_; }
        int y() const { return y_; }

        void SetPoint(int x, int y)
        {
            x_ = x;
            y_ = y;
        }

        void set_x(int x) { x_ = x; }
        void set_y(int y) { y_ = y; }

        void Offset(int delta_x, int delta_y)
        {
            x_ += delta_x;
            y_ += delta_y;
        }

        Point Add(const Point& other) const
        {
            Point copy = *this;
            copy.Offset(other.x_, other.y_);
            return copy;
        }

        Point Subtract(const Point& other) const
        {
            Point copy = *this;
            copy.Offset(-other.x_, -other.y_);
            return copy;
        }

        bool operator==(const Point& rhs) const
        {
            return x_ == rhs.x_ && y_ == rhs.y_;
        }

        bool operator!=(const Point& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const Point& rhs) const
        {
            return (y_ == rhs.y_) ? (x_ < rhs.x_) : (y_ < rhs.y_);
        }

        POINT ToPOINT() const;

    private:
        int x_;
        int y_;
    };

    std::ostream& operator<<(std::ostream& out, const gfx::Point& p);

} //namespace gfx

#endif //__ui_gfx_point_h__
