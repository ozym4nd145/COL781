#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"
Box::Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
         float b, float h, const Material& mat)
    : Model{mat},
      _center{center},
      _l{l},
      _b{b},
      _h{h},
      _ax{x.normalized()},
      _az{(_ax.cross(y)).normalized()},
      _ay{(_az.cross(_ax)).normalized()},
      _coll{mat} {
    float l2 = _l / 2;
    float b2 = _b / 2;
    float h2 = _h / 2;
    Point c = _center;
    Point ubl = c + _az * b2 + _ay * h2 - _ax * l2;  // upper bottom left
    Point ubr = c + _az * b2 + _ay * h2 + _ax * l2;  // upper bottom right
    Point utl = c - _az * b2 + _ay * h2 - _ax * l2;  // upper top left
    Point utr = c - _az * b2 + _ay * h2 + _ax * l2;  // upper top right
    Point lbl = c + _az * b2 - _ay * h2 - _ax * l2;  // lower bottom left
    Point lbr = c + _az * b2 - _ay * h2 + _ax * l2;  // lower bottom right
    Point ltl = c - _az * b2 - _ay * h2 - _ax * l2;  // lower top left
    Point ltr = c - _az * b2 - _ay * h2 + _ax * l2;  // lower top right

    // top face
    _coll.addModel(new Triangle(utl, utr, ubr, mat));
    _coll.addModel(new Triangle(utl, ubl, ubr, mat));
    // bottom face
    _coll.addModel(new Triangle(ltl, ltr, lbr, mat));
    _coll.addModel(new Triangle(ltl, lbl, lbr, mat));
    // front face
    _coll.addModel(new Triangle(ubl, ubr, lbr, mat));
    _coll.addModel(new Triangle(ubl, lbl, lbr, mat));
    // back face
    _coll.addModel(new Triangle(utl, utr, ltr, mat));
    _coll.addModel(new Triangle(utl, ltl, lbr, mat));
    // left face
    _coll.addModel(new Triangle(ubl, utl, ltl, mat));
    _coll.addModel(new Triangle(ubl, lbl, ltl, mat));
    // right face
    _coll.addModel(new Triangle(ubr, utr, ltr, mat));
    _coll.addModel(new Triangle(ubr, lbr, ltr, mat));
}

std::optional<std::pair<float, const Model*>> Box::getIntersectionLengthAndPart(
    const Ray& r) const {
    return _coll.getIntersectionLengthAndPart(r);
}

bool Box::isOnSurface(const Point& p) const { return _coll.isOnSurface(p); }

std::optional<Ray> Box::getNormal(const Point& p) const {
    return _coll.getNormal(p);
}

std::ostream& Box::print(std::ostream& os) const {
    return os << "Box{l=" << _l << ",b=" << _b << ",h=" << _h << ",ax=" << _ax
              << ",ay=" << _ay << ",az=" << _az << "}";
}