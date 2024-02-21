#include "hittable.hpp"


HittableList::HittableList(std::shared_ptr<Hittable> object)
{
    add(object);
}


void HittableList::clear()
{
    objects.clear();
}


void HittableList::add(std::shared_ptr<Hittable> object)
{
    objects.push_back(object);
}

std::optional<Hit> HittableList::trace(const Ray & r, float t_min, float t_max) const
{
    std::optional<Hit> hit = std::nullopt;

    auto closest_so_far = t_max;
    
    for (const auto & object : objects) {
        if (auto hit_tmp = object -> trace(r, t_min, closest_so_far)) {
            closest_so_far = hit_tmp -> solution;
            hit = hit_tmp;
        }
    }
    
    return hit;
}

