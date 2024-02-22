#include "sphere.hpp"


#include <cmath>


Sphere::Sphere(const Point3 & center, const double & radius, std::shared_ptr<Material> material) :
    center_(center),
    radius_(radius),
    material_(material)
{}

Point3 Sphere::center() const
{
    return center_;
}

double Sphere::radius() const
{
    return radius_;
}
    
std::optional<Hit> Sphere::trace(const Ray & r, double t_min, double t_max) const
{
    Vec3 oc = r.origin() - center_;

    double a = r.direction().norm_squared();
    double b = dot(oc, r.direction());
    double c = oc.norm_squared() - radius_ * radius_;
    double d = b * b - a * c;

    if (d < 0)
        return std::nullopt;

    double d_sqrt = std::sqrt(d);
    double t = (-b - d_sqrt) / a;

    if (t < t_min || t_max < t) {
        t = (-b + d_sqrt) / a;

        if (t < t_min || t_max < t)
            return std::nullopt;
    }

    Point3 p = r(t);
    Vec3 n_out = (p - center_) / radius_;
    bool front_face = dot(r.direction(), n_out) < 0;

    return Hit { p, front_face ? n_out : -n_out, t, front_face, material_ };
}
