#pragma once

#include <iostream>
#include <math.h>

template<typename T>
class Triple {
    private:
        T _x,_y,_z;
    public:
        const T& x;
        const T& y;
        const T& z;
        Triple(T v): _x{v},_y{v},_z{v},x{_x},y{_y},z{_z} {}
        Triple(T x,T y,T z): _x{x},_y{y},_z{z},x{_x},y{_y},z{_z} {}
        Triple(const Triple<T>& v): _x{v.x},_y{v.y},_z{v.z},x{_x},y{_y},z{_z} { }
        virtual ~Triple() = default;
        Triple<T>& operator=(const Triple<T>& o) {
            _x=o.x;_y=o.y;_z=o.z; return *this;
        }
        Triple<T> operator-() const {
            return Triple<T>(-x,-y,-z);
        }
        friend Triple<T> operator+(const Triple<T>& l, const Triple<T>& r) {
            return Triple<T>(l.x + r.x, l.y + r.y, l.z + r.z);
        }
        friend Triple<T> operator-(const Triple<T>& l, const Triple<T>& r) {
            return Triple<T>(l.x - r.x, l.y - r.y, l.z - r.z);
        }
        friend Triple<T> operator*(const Triple<T>& l, const Triple<T>& r) {
            return Triple<T>(l.x * r.x, l.y * r.y, l.z * r.z);
        }
        Triple<T>& operator+=(const Triple<T>& o) {
            _x+=o.x; _y+=o.y; _z+=o.z; return *this;
        }
        Triple<T>& operator*=(const Triple<T>& o) {
            _x*=o.x; _y*=o.y; _z*=o.z; return *this;
        }
        friend Triple<T> operator+(const Triple<T>& l, const T& r) {
            return Triple<T>(l.x + r, l.y + r, l.z + r);
        }
        friend Triple<T> operator-(const Triple<T>& l, const T& r) {
            return Triple<T>(l.x - r, l.y - r, l.z - r);
        }
        friend Triple<T> operator*(const Triple<T>& l, const T& r) {
            return Triple<T>(l.x * r, l.y * r, l.z * r);
        }
        friend Triple<T> operator/(const Triple<T>& l, const T& r) {
            return Triple<T>(l.x / r, l.y / r, l.z / r);
        }
        friend Triple<T> operator+(const T& l, const Triple<T>& r) {
            return r+l;
        }
        friend Triple<T> operator-(const T& l, const Triple<T>& r) {
            return -(r-l);
        }
        friend Triple<T> operator*(const T& l, const Triple<T>& r) {
            return r*l;
        }
        friend Triple<T> operator/(const T& l, const Triple<T>& r) {
            return inv(l)*r;
        }
        Triple<T>& operator+=(const T& o) {
            _x+=o; _y+=o; _z+=o; return *this;
        }
        Triple<T>& operator*=(const T& o) {
            _x*=o; _y*=o; _z*=o; return *this;
        }
        T dot(const Triple<T>& o) const {
            return o.x*x + o.y*y + o.z*z;
        }
        Triple<T> inv() const {
            return Triple<T>(1/x,1/y,1/z);
        }
        T length() const {
            return sqrt(x*x+y*y+z*z);
        }
        Triple<T>& normalize() {
            T length = this->length();
            if(length > 0) {
                _x = x/length; _y = y/length; _z = z/length;
            }
            return *this;
        }
        Triple<T> normalize() const {
            T length = this->length();
            if(length > 0) {
                return Triple<T>(x/length,y/length,z/length);
            }
            return *this;
        }
        virtual std::ostream& print(std::ostream& os) const {
            return os<<"("<<x<<","<<y<<","<<z<<")";
        }
        friend std::ostream& operator<<(std::ostream& os, const Triple<T>& v) {
            return v.print(os);
        }
};

class Point: public Triple<float> {
    typedef Triple<float> Parent;
    public:
        using Parent::Parent;
        Point(const Parent& v): Parent(v) { }
        std::ostream& print(std::ostream& os) const override {
            return Parent::print(os<<"Point");
        }

};

class Color: public Triple<float> {
    typedef Triple<float> Parent;
    public:
        using Parent::Parent;
        Color(const Parent& v): Parent(v) { }
        std::ostream& print(std::ostream& os) const override {
            return Parent::print(os<<"Color");
        }
};

class Ray {
    typedef Triple<float> Vec;
    private:
        Point _src;
        Vec _dir;
        float _length;
    public:
        const Point& src;
        const Vec& dir;
        const float& length;
        Ray(const Point& source,const Vec& direction): _src{source},_dir{direction.normalize()},_length{direction.length()},src{_src},dir{_dir},length{_length} {}
        Ray(const Ray& r): _src{r.src},_dir{r.dir},_length{r.length},src{_src},dir{_dir},length{_length} {}
        Ray& operator=(const Ray& o) {
            _src=o.src;_dir=o.dir;_length=o.length; return *this;
        }
        friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
            return os<<"Ray{"<<r.src<<"->"<<r.dir<<"}";
        }
};
