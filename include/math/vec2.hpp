#ifndef VEC2_HPP
#define VEC2_HPP

#include <type_traits>
#include <iostream>
#include <math.h>

template <typename T>
class vec2
{
public:
    T x, y;

    vec2() : x(static_cast<T>(0)), y(static_cast<T>(0)) { this->_magnitude = 0; }
    vec2(T x, T y) : x(x), y(y) { this->_upd_magnitude(); }
    ~vec2() {}

    template <typename U>
    explicit vec2(const vec2<U> &other)
        : x(static_cast<T>(other.x)),
          y(static_cast<T>(other.y)) { this->_upd_magnitude(); }

    // remove later, this was just for testing stuff! or maybe not idk
    void print_values() { std::cout << x << " " << y << std::endl; }

    float magnitude() const { return this->_magnitude; }

    // operations

    vec2<T> vec2<T>::operator+(const vec2<T> &other) { return vec2<T>(this->x + other.x, this->y + other.y); }

    vec2<T> vec2<T>::operator-(const vec2<T> &other) { return vec2<T>(this->x - other.x, this->y - other.y); }

    vec2<T> vec2<T>::operator*(const T &scalar) { return vec2<T>(this->x * scalar, this->y * scalar); }

    vec2<T> vec2<T>::operator/(const T &scalar) { return vec2<T>(this->x / scalar, this->y / scalar); }

    vec2<T> &vec2<T>::operator+=(const vec2<T> &other)
    {
        this->x += other.x;
        this->y += other.y;

        this->_upd_magnitude();

        return *this;
    }

    vec2<T> &vec2<T>::operator-=(const vec2<T> &other)
    {
        this->x -= other.x;
        this->y -= other.y;

        this->_upd_magnitude();

        return *this;
    }

    vec2<T> &vec2<T>::operator*=(const T &scalar)
    {
        this->x *= scalar;
        this->y *= scalar;

        this->_upd_magnitude();

        return *this;
    }

    vec2<T> &vec2<T>::operator/=(const T &scalar)
    {
        this->x /= scalar;
        this->y /= scalar;

        this->_upd_magnitude();

        return *this;
    }

private:
    static_assert(std::is_arithmetic<T>::value, "Provided type is not a number!");
    float _magnitude;

    void _upd_magnitude() { this->_magnitude = std::sqrt(this->x * this->x + this->y * this->y); }
};

#endif