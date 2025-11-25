#ifndef NEIGHBORS_HPP
#define NEIGHBORS_HPP

#include <cstdint>
#include <array>
#include "Vector.hpp"

namespace wfc 
{

template <uint8_t D> struct Neighbors;

template <> struct Neighbors<2>
{
    static constexpr std::array<std::array<int8_t, 2>, 4> neighbors = {
        {
            {0,   1}, // up
            {0,  -1}, // down
            {1,   0}, // right
            {-1,  0}, // left
        }
    };
};

template <> struct Neighbors<3>
{
    static constexpr std::array<std::array<int8_t, 3>, 6> neighbors = {
        {
            {0,   1,  0}, // up
            {0,  -1,  0}, // down
            {1,   0,  0}, // right
            {-1,  0,  0}, // left
            {0,   0,  1}, // forward
            {0,   0, -1} // backward
        }
    };
};

};

#endif // NEIGHBORS_HPP