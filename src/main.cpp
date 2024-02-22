#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <ranges>
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
    float t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * ColorRGB(1.0, 1.0, 1.0) + t * ColorRGB(0.5, 0.7, 1.0);
}


HittableList random_scene()
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(ColorRGB(18, 255, 219) / 255);
    world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_real_distribution<float> uniform(0.0, 1.0);

    for (int a = -10; a <= 10; ++a) {
        for (int b = -10; b <= 10; ++b) {

            if (
                std::pow(a, 2) + std::pow(b, 2) < 2 ||
                std::pow(a - 3, 2) + std::pow(b, 2) < 2 ||
                std::pow(a, 2) + std::pow(b - 3, 2) < 2)
                continue;

            float random_value = uniform(gen);
            float radius = 0.125 + random_value * 0.25;

            Point3 center(a + 0.5 * uniform(gen), radius, b + 0.5 * uniform(gen));

            if ((center - Point3(4, 0.2, 0)).norm() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (random_value < 0.8) {
                    // diffuse
                    auto albedo = 
                        ColorRGB(uniform(gen), uniform(gen), uniform(gen)) *
                        ColorRGB(uniform(gen), uniform(gen), uniform(gen));

                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, radius, sphere_material));
                } else if (random_value < 0.95) {
                    // metal
                    auto albedo = ColorRGB(
                        0.5 + 0.5 * uniform(gen), 0.5 + 0.5 * uniform(gen), 0.5 + 0.5 * uniform(gen));

                    auto fuzz = 0.5 + 0.5 * uniform(gen);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, radius, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, radius, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3(3, 1, 0), 1, material1));

    auto material2 = std::make_shared<Lambertian>(ColorRGB(248, 15, 17) / 255);
    world.add(std::make_shared<Sphere>(Point3(0, 1, 3), 1, material2));

    auto material3 = std::make_shared<Metal>(ColorRGB(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3(0, 1, 0), 1, material3));

    return world;
}


int main()
{
    const float aspect_ratio = 16.0 / 9.0;
    const int image_h = 720;
    const int image_w = static_cast<int>(image_h * aspect_ratio);

    unsigned int n_samples = 16;
    unsigned int bounces = 16;
    unsigned int n_threads = std::thread::hardware_concurrency();
    
    std::cout << "Number of threads: " << n_threads << std::endl;

    Image<float, 3> img(image_w, image_h, { 0, 0, 0 });

    // Camera
    Point3 look_from(10, 2, 6);
    Point3 look_at(0, 0, 0);
    Vec3 up(0, 1, 0);
    auto dist_to_focus = (look_from - look_at).norm();
    auto aperture = 1 / 5.6;

    Camera cam(look_from, look_at, up, 20, aspect_ratio, aperture, dist_to_focus);

    // Objects
    HittableList objects = random_scene();

    // Render loop
    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    std::cout << "Rendering..." << std::endl;

    for (unsigned int thread_id = 0; thread_id < n_threads; ++thread_id)
        threads.push_back(
            std::thread([&objects, &img, &cam, image_h, image_w, n_threads, thread_id, n_samples, bounces] () {

                std::random_device rd;
                std::minstd_rand gen(rd());
                std::uniform_real_distribution<float> uniform(0.0, 1.0);

                auto work_group =
                    std::views::iota(0, image_h * image_w) |
                    std::views::filter([n_threads, thread_id] (int i) { return i % n_threads == thread_id; });

                for (int pixel_id : work_group) {
                    int i = pixel_id / image_w;
                    int j = pixel_id % image_w;

                    ColorRGB c = { 0, 0, 0 };

                    for (unsigned int s = 0; s < n_samples; ++s) {
                        float u = (float(j) + uniform(gen)) / (image_w - 1);
                        float v = (image_h - 1 - float(i) + uniform(gen)) / (image_h - 1);

                        Ray r = cam.get_ray(u, v);

                        // Color calculation
                        c += ray_color(r, objects, bounces);
                    }

                    img(i, j) = sqrt(c / n_samples);
                }
            })
        );

    for (auto & t : threads)
        t.join();

    /*
    int n_samples = 64;
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            ColorRGB c = { 0, 0, 0 };

            for (int s = 0; s < n_samples; ++s) {
                float u = (float(j) + uniform(gen)) / (image_width - 1);
                float v = (image_height - 1 - float(i) + uniform(gen)) / (image_height - 1);

                Ray r = cam.get_ray(u, v);

                // Color calculation
                c += ray_color(r, objects, 8);
            }

            c /= n_samples;
            img(i, j) = sqrt(c);
        }

        std::cerr << "\rScanlines remaining: " << (image_height - i - 1) << ' ' << std::flush;
    }
    */

    std::cout << "Render Finished!" << std::endl;

    img.save("img.png");
}
