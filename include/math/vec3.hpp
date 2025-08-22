#ifndef VEC3_HPP
#define VEC3_HPP

#include <type_traits>
#include <iostream>
#include <math.h>

template <typename T>
class vec3
{
public:
    T x, y, z;

    vec3() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) { this->_magnitude = 0; }
    vec3(T x, T y, T z) : x(x), y(y), z(z) { this->_upd_magnitude(); }
    ~vec3() {}

    template <typename U>
    explicit vec3(const vec3<U> &other)
        : x(static_cast<T>(other.x)),
          y(static_cast<T>(other.y)),
          z(static_cast<T>(other.z)) { this->_upd_magnitude(); }

    // remove later, this was just for testing stuff! or maybe not idk
    void print_values() { std::cout << x << " " << y << " " << z << std::endl; }

    static float dot(const vec3<T> &v1, const vec3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
    static vec3<T> cross(const vec3<T> &v1, const vec3<T> &v2)
    {
        return vec3<T>(v1.y * v2.z - v1.z * v2.y,
                       v1.z * v2.x - v1.x * v2.z,
                       v1.x * v2.y - v1.y * v2.x);
    }

    float magnitude() const { return this->_magnitude; }
    void normalize()
    {
        if (this->_magnitude > 0)
            *this /= this->_magnitude;
    }

    // operations

    vec3<T> vec3<T>::operator+(const vec3<T> &other) { return vec3<T>(this->x + other.x, this->y + other.y, this->z + other.z); }

    vec3<T> vec3<T>::operator-(const vec3<T> &other) { return vec3<T>(this->x - other.x, this->y - other.y, this->z - other.z); }

    vec3<T> vec3<T>::operator*(const T &scalar) { return vec3<T>(this->x * scalar, this->y * scalar, this->z * scalar); }

    vec3<T> vec3<T>::operator/(const T &scalar) { return vec3<T>(this->x / scalar, this->y / scalar, this->z / scalar); }

    vec3<T> &vec3<T>::operator+=(const vec3<T> &other)
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;

        this->_upd_magnitude();

        return *this;
    }

    vec3<T> &vec3<T>::operator-=(const vec3<T> &other)
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;

        this->_upd_magnitude();

        return *this;
    }

    vec3<T> &vec3<T>::operator*=(const T &scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;

        this->_upd_magnitude();

        return *this;
    }

    vec3<T> &vec3<T>::operator/=(const T &scalar)
    {
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;

        this->_upd_magnitude();

        return *this;
    }

private:
    static_assert(std::is_arithmetic<T>::value, "Provided type is not a number!");
    float _magnitude;

    void _upd_magnitude() { this->_magnitude = std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z); }
};

#endif