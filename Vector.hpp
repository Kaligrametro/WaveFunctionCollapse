#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstdint>
namespace wfc 
{
    template <uint8_t D> struct Vector;

    template <> struct Vector<2> {
        Vector() : x(0), y(0) {}
        Vector(const int x_, const int y_) : x(x_), y(y_) {}
        int x, y;
    };

    template <> struct Vector<3> {
        Vector() : x(0), y(0), z(0) {}
        Vector(const int x_, const int y_, const int z_) : x(x_), y(y_), z(z_) {}
        int x, y, z;
    };
};

#endif // VECTOR_HPP