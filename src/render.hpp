#ifndef RENDER_HPP
#define RENDER_HPP


#include "image.hpp"
#include "hittable.hpp"
#include "camera.hpp"
#include "ray.hpp"


class Render {
public:

    /*
    Camera cam;
    HittableList objects;

    Render renderer(cam, objects);

    for (int i = 0; i < 100; ++i) {
        img = renderer.render();
        img.save(...)

        cam.update(...)
        objects.update(...)
    }
    */

    Render(Camera & camera, HittableList & objects) : 
        camera_(camera),
        objects_(objects)
    {}

    Image<float, 3> render();

private:

    Camera & camera_;
    HittableList & objects_;
};


#endif // RENDER_HPP