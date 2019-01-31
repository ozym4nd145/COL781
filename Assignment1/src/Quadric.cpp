#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"
#include "utils.h"

std::optional<std::pair<float, const Model*>>
Quadric::getIntersectionLengthAndPart(const Ray& r) const {
    auto Ro = augment(r.src, 1.0);
    auto Rd = augment(r.dir, 0.0);
    float Aq = Rd * M * (Rd.transpose());
    float Bq = Rd * M * (Ro.transpose());
    Bq += Ro * M * (Rd.transpose());
    float Cq = Ro * M * (Ro.transpose());
    auto inters = solve_quadratic(Aq, Bq, Cq);
    if (!inters) return {};
    auto min_pos_dist = (inters.value().first < 0) ? (inters.value().second)
                                                   : (inters.value().first);
    if (min_pos_dist < 0) return {};
    return std::make_pair(min_pos_dist, this);
}

bool Quadric::isOnSurface(const Point& p) const {
    float val = augment(p, 1.0) * M * (augment(p, 1.0).transpose());
    return (abs(val) <= 10 * EPSILON);
}

std::optional<Ray> Quadric::getNormal(const Point& p) const {
    if (!isOnSurface(p)) return {};
    float nx = 2.0 * (_qp.A * p[0] + _qp.B * p[1] + _qp.C * p[2] + _qp.D);
    float ny = 2.0 * (_qp.B * p[0] + _qp.E * p[1] + _qp.F * p[2] + _qp.G);
    float nz = 2.0 * (_qp.C * p[0] + _qp.F * p[1] + _qp.H * p[2] + _qp.I);
    return Ray(p, Vector3f(nx, ny, nz));
}

std::ostream& Quadric::print(std::ostream& os) const {
    return os << "Quadric{Matrix=" << std::endl << M << std::endl << "}";
}
