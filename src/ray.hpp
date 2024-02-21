#ifndef RAY_HPP
#define RAY_HPP

#include "vec.hpp"

class Ray {
public:
    Ray(const Point3 & origin, const Vec3 & direction) :
        origin_(origin),
        direction_(direction)
    {}

    Point3 origin() const { return origin_; }
    Vec3 direction() const { return direction_; }

    Point3 operator() (float t) const {
        return origin_ + t * direction_;
    }

private:
    Point3 origin_;
    Vec3 direction_;
};

#endif // RAY_HPP