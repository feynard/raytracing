#include "material.hpp"


Lambertian::Lambertian(const ColorRGB & albedo, const unsigned int & seed) :
    albedo_(albedo),
    gen_(seed),
    gaussian_(0.0, 1.0)
{}

std::optional<std::tuple<ColorRGB, Ray>> Lambertian::scatter(const Ray & r, const Hit & hit) const
{
    Vec3 scatter_direction = hit.normal + random_unit();

    if (scatter_direction.near_zero())
        scatter_direction = hit.normal;

    return std::tuple { albedo_, Ray(hit.point, scatter_direction) };
}


Metal::Metal(const ColorRGB & albedo, const double & fuzz, const unsigned int & seed) :
    albedo_(albedo),
    fuzz_(fuzz < 1 ? fuzz : 1),
    gen_(seed),
    gaussian_(0.0, 1.0)
{}

std::optional<std::tuple<ColorRGB, Ray>> Metal::scatter(const Ray & r, const Hit & hit) const 
{
    Vec3 reflected = unit(r.direction()).reflect(hit.normal);
    Ray scattered = Ray(hit.point, reflected + fuzz_ * random_unit());
    ColorRGB attenuation = albedo_;

    if (dot(scattered.direction(), hit.normal) > 0)
        return std::tuple { attenuation, scattered };
    else
        return std::nullopt;
}


Dielectric::Dielectric(const double & ir, const unsigned int & seed) :
    ir_(ir),
    gen_(seed),
    uniform_(0.0, 1.0)
{}

std::optional<std::tuple<ColorRGB, Ray>> Dielectric::scatter(const Ray & r, const Hit & hit) const
{
    ColorRGB attenuation(1.0, 1.0, 1.0);
    double refraction_ratio = hit.front_face ? (1.0 / ir_) : ir_;

    Vec3 unit_direction = unit(r.direction());
    double cos_theta = std::min(dot(-unit_direction, hit.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > uniform_(gen_))
        direction = unit_direction.reflect(hit.normal);
    else
        direction = refract(unit_direction, hit.normal, refraction_ratio);

    Ray scattered(hit.point, direction);
    
    return std::tuple { attenuation, scattered };
}

Vec3 Dielectric::refract(const Vec3 & unit_direction, const Vec3 & n, const double & refraction_ratio) const
{
    auto cos_theta = std::min(dot(-unit_direction, n), 1.0);
    
    Vec3 r_out_perp = refraction_ratio * (unit_direction + cos_theta*n);
    Vec3 r_out_parallel = -std::sqrt(fabs(1.0 - r_out_perp.norm_squared())) * n;
    
    return r_out_perp + r_out_parallel;
}

double Dielectric::reflectance(const double & cosine, const double & ref_idx)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}
