#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "vec.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "material.hpp"

#include <optional>


class Sphere : public Hittable {
public:
    Sphere(const Point3 & center, const double & radius, std::shared_ptr<Material> material);
    Point3 center() const;
    double radius() const;
    
    virtual std::optional<Hit> trace(const Ray & r, double t_min, double t_max) const override;

private:

    Point3 center_;
    double radius_;
    std::shared_ptr<Material> material_;
};

#endif // SPHERE_HPP