#ifndef HITTABLE_HPP
#define HITTABLE_HPP


#include "vec.hpp"
#include "ray.hpp"
#include "material.hpp"


#include <tuple>
#include <optional>
#include <vector>
#include <memory>


class Material;

struct Hit {
    Point3 point;
    Vec3 normal;
    double solution;
    bool front_face;
    std::shared_ptr<Material> material;
};

// hit point, normal, solution for a ray, front face
// using Hit = std::tuple<Point3, Vec3, double, bool>;

class Hittable {
public:

    virtual std::optional<Hit> trace(const Ray & r, double t_min, double t_max) const = 0;

    virtual ~Hittable() = default;
};


class HittableList : public Hittable {
public:
    HittableList() = default;

    HittableList(std::shared_ptr<Hittable> object);

    void clear();
    
    void add(std::shared_ptr<Hittable> object);

    virtual std::optional<Hit> trace(const Ray & r, double t_min, double t_max) const override;

private:
    std::vector<std::shared_ptr<Hittable>> objects;
};


#endif // HITTABLE_HPP