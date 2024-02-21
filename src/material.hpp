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
    mutable std::normal_distribution<float> gaussian_;

    Vec3 random_unit() const {
        return unit(Vec3(gaussian_(gen_), gaussian_(gen_), gaussian_(gen_)));
    };
};


class Metal : public Material {
public:
    
    Metal(const ColorRGB & albedo, const float & fuzz, const unsigned int & seed = 0);

    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const override;

private:

    ColorRGB albedo_;
    float fuzz_;

    mutable std::minstd_rand gen_;
    mutable std::normal_distribution<float> gaussian_;

    Vec3 random_unit() const {
        return unit(Vec3(gaussian_(gen_), gaussian_(gen_), gaussian_(gen_)));
    };
};


class Dielectric : public Material {
public:

    Dielectric(const float & ir, const unsigned int & seed = 0);

    virtual std::optional<std::tuple<ColorRGB, Ray>> scatter(const Ray & r, const Hit & hit) const override;

private:

    Vec3 refract(const Vec3 & uv, const Vec3 & n, const float & refraction_ratio) const;

    static float reflectance(const float & cosine, const float & ref_idx);

    float ir_;

    mutable std::minstd_rand gen_;
    mutable std::uniform_real_distribution<float> uniform_;
};


#endif // MATERIAL_HPP
