#ifndef CAMERA_HPP
#define CAMERA_HPP


#include "ray.hpp"
#include "vec.hpp"


#include <cmath>


class Camera {
public:
    
    Camera(
        const Point3 & look_from,
        const Point3 & look_at,
        const Vec3 & up,
        const double & fov, // vertical field-of-view in degrees
        const double & aspect_ratio,
        const double & aperture,
        const double & focus_dist,
        const int & seed = 0
    ) :
        gen_(seed),
        gaussian_(0, 1),
        uniform_(0, 1)
    {
        double theta = fov / 180.0 * 3.1415926;
        double h = std::tan(theta / 2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;

        w_ = unit(look_from - look_at);
        u_ = unit(cross(up, w_));
        v_ = cross(w_, u_);

        origin_ = look_from;
        horizontal_ = focus_dist * viewport_width * u_;
        vertical_ = focus_dist * viewport_height * v_;
        lower_left_corner_ = origin_ - horizontal_ / 2 - vertical_ / 2 - focus_dist * w_;
        lens_radius_ = aperture / 2;
    }

        Ray get_ray(double s, double t) const
        {
            // return Ray(origin_, lower_left_corner_ + s * horizontal_ + t * vertical_ - origin_);

            Vec2 rd = lens_radius_ * random_in_unit_disk();
            Vec3 offset = u_ * rd.x + v_ * rd.y;

            return Ray(origin_ + offset, lower_left_corner_ + s * horizontal_ + t * vertical_ - origin_ - offset);

        }


private:

    Point3 origin_;
    Point3 lower_left_corner_;
    Vec3 horizontal_;
    Vec3 vertical_;
    Vec3 u_, v_, w_;
    double lens_radius_;

    mutable std::minstd_rand gen_;
    mutable std::normal_distribution<double> gaussian_;
    mutable std::uniform_real_distribution<double> uniform_;

    Vec2 random_in_unit_disk() const {
        Vec2 r(gaussian_(gen_), gaussian_(gen_));
        return r / r.norm() * std::sqrt(uniform_(gen_));
    }
};


#endif //CAMERA_HPP
