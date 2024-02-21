#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <memory>
#include <cmath>

#include "vec.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "hittable.hpp"
#include "camera.hpp"


ColorRGB ray_color(const Ray & r, const Hittable & objects, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return { 0, 0, 0 };
    
    if (auto hit = objects.trace(r, 0.0001, std::numeric_limits<float>::infinity())) {
        if (auto scattered = hit -> material -> scatter(r, *hit)) {
            auto [attenuation, scattered_ray] = *scattered;
            return attenuation * ray_color(scattered_ray, objects, depth - 1);
        }

        return { 0, 0, 0 };
    }

    Vec3 unit_direction = unit(r.direction());
    auto t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * ColorRGB(1.0, 1.0, 1.0) + t * ColorRGB(0.5, 0.7, 1.0);
}


HittableList random_scene()
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(ColorRGB(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, ground_material));

    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_real_distribution<float> uniform(0.0, 1.0);

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = uniform(gen);
            Point3 center(a + 0.9 * uniform(gen), 0.2, b + 0.9 * uniform(gen));

            if ((center - Point3(4, 0.2, 0)).norm() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = 
                        ColorRGB(uniform(gen), uniform(gen), uniform(gen)) *
                        ColorRGB(uniform(gen), uniform(gen), uniform(gen));

                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = ColorRGB(
                        0.5 + 0.5 * uniform(gen), 0.5 + 0.5 * uniform(gen), 0.5 + 0.5 * uniform(gen));

                    auto fuzz = 0.5 + 0.5 * uniform(gen);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(ColorRGB(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(ColorRGB(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}


int main()
{
    // Random 
    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_real_distribution<float> uniform(0.0, 1.0);
    
    // Image
    const float aspect_ratio = 16.0 / 9.0;
    const int image_height = 360;
    const int image_width = static_cast<int>(image_height * aspect_ratio);

    Image<float, 3> img(image_width, image_height, {0, 0, 0});

    // Camera
    Point3 look_from(8, 2, 8);
    Point3 look_at(0, 0, -1);
    Vec3 up(0, 1, 0);
    auto dist_to_focus = (look_from - look_at).norm();
    auto aperture = 0.25;

    Camera cam(look_from, look_at, up, 20, aspect_ratio, aperture, dist_to_focus);


    // Objects
    HittableList objects = random_scene();

    /*
    auto material_ground = std::make_shared<Lambertian>(ColorRGB(0.4, 0.8, 0.2));
    auto material_center = std::make_shared<Lambertian>(ColorRGB(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<Dielectric>(1.5);
    auto material_right  = std::make_shared<Metal>(ColorRGB(0.8, 0.6, 0.2), 0.0);

    objects.add(std::make_shared<Sphere>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    objects.add(std::make_shared<Sphere>(Point3( 0.0,    0.0, -1.0),   0.5, material_center));
    objects.add(std::make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
    objects.add(std::make_shared<Sphere>(Point3(-1.0,    0.0, -1.0), -0.45, material_left));
    objects.add(std::make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));
    */

    // Render loop
    int n_samples = 16;
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            ColorRGB c = { 0, 0, 0 };

            for (int s = 0; s < n_samples; ++s) {
                float u = (float(j) + uniform(gen)) / (image_width - 1);
                float v = (image_height - 1 - float(i) + uniform(gen)) / (image_height - 1);

                Ray r = cam.get_ray(u, v);

                // Color calculation
                 c += ray_color(r, objects, 8) / n_samples;
            }

            img(i, j) = Color<float, 3>(sqrt(c)); // Can be made without explicit casting
        }

        std::cerr << "\rScanlines remaining: " << (image_height - i - 1) << ' ' << std::flush;
    }

    std::cout << std::endl << "Render Finished!" << std::endl;

    img.save("img.png");
}
