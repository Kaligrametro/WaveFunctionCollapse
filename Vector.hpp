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

        void iterate(
            const std::function<void(Vector<2>)>& func, 
            const int advance = 1) const
        {
            for (int _x = 0; _x < this->x + 1; _x += advance)
            for (int _y = 0; _y < this->y + 1; _y += advance)
                func(Vector<2>(_x, _y));
        }

        static bool inBounds(const Vector<2>& size, const Vector<2>& position) 
        {
            return (position.x >= 0 && position.x < size.x &&
                    position.y >= 0 && position.y < size.y);
        }

        size_t size() const
        {
            return (size_t)(this->x * this->y);
        }
    };

    template <> struct Vector<3> {
        Vector() : x(0), y(0), z(0) {}
        Vector(const int x_, const int y_, const int z_) : x(x_), y(y_), z(z_) {}
        int x, y, z;

        void iterate(
            const std::function<void(Vector<3>)>& func, 
            const int advance = 1) const
        {
            for (int _x = 0; _x < this->x + 1; _x += advance)
            for (int _y = 0; _y < this->y + 1; _y += advance)
            for (int _z = 0; _z < this->z + 1; _z += advance)
                func(Vector<3>(_x, _y, _z));
        }

        static bool inBounds(const Vector<3>& size, const Vector<3>& position) 
        {
            return (position.x >= 0 && position.x < size.x &&
                    position.y >= 0 && position.y < size.y &&
                    position.z >= 0 && position.z < size.z);
        }

        size_t size() const
        {
            return (size_t)(this->x * this->y * this->z);
        }
    };
};

#endif // VECTOR_HPP