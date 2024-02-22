#ifndef MATERIAL_HPP
#define MATERIAL_HPP


#include "hittable.hpp"
#include "ray.hpp"
#include "vec.hpp"

#include <tuple>
#include <random>
#include <optional>


struct Hit;


class Material {
public:
    // Returns attenuation color and scattered ray
    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const = 0;

    virtual ~Material() = default;
};


class Lambertian : public Material {
public:
    Lambertian(const ColorRGB & albedo, const unsigned int & seed = 0);

    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const override;

private:

    ColorRGB albedo_;

    mutable std::minstd_rand gen_;
    mutable std::normal_distribution<double> gaussian_;

    Vec3 random_unit() const {
        return unit(Vec3(gaussian_(gen_), gaussian_(gen_), gaussian_(gen_)));
    };
};


class Metal : public Material {
public:
    
    Metal(const ColorRGB & albedo, const double & fuzz, const unsigned int & seed = 0);

    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const override;

private:

    ColorRGB albedo_;
    double fuzz_;

    mutable std::minstd_rand gen_;
    mutable std::normal_distribution<double> gaussian_;

    Vec3 random_unit() const {
        return unit(Vec3(gaussian_(gen_), gaussian_(gen_), gaussian_(gen_)));
    };
};


class Dielectric : public Material {
public:

    Dielectric(const double & ir, const unsigned int & seed = 0);

    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const override;

private:

    Vec3 refract(const Vec3 & uv, const Vec3 & n, const double & refraction_ratio) const;

    static double reflectance(const double & cosine, const double & ref_idx);

    double ir_;

    mutable std::minstd_rand gen_;
    mutable std::uniform_real_distribution<double> uniform_;
};


#endif // MATERIAL_HPP
