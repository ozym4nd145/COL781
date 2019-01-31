#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

std::optional<const Model*> Collection::getWhichPart(const Point& p) const {
    for (auto part : _parts) {
        if (part->isOnSurface(p)) return part;
    }
    return {};
}

std::optional<std::pair<float, const Model*>>
Collection::getIntersectionLengthAndPart(const Ray& r) const {
    const Model* closest_model_part = NULL;
    float closest_distance = std::numeric_limits<float>::infinity();

    for (auto part : _parts) {
        auto intersection_part = part->getIntersectionLengthAndPart(r);
        if (!intersection_part) continue;
        auto len = intersection_part.value().first;
        if (len < closest_distance) {
            closest_distance = len;
            closest_model_part = part;
        }
    }

    if (!closest_model_part) return {};
    return std::make_pair(closest_distance, closest_model_part);
}

bool Collection::isOnSurface(const Point& p) const {
    return getWhichPart(p).has_value();
}

std::optional<Ray> Collection::getNormal(
    const Point& p) const {  // returns outward normal
    auto part_opt = getWhichPart(p);
    if (!part_opt) return {};
    auto part = part_opt.value();
    return part->getNormal(p);
}

std::ostream& Collection::print(std::ostream& os) const {
    return os << "Collection{num_parts=" << _parts.size() << "}";
}