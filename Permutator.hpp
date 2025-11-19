#ifndef PERMUTATOR_HPP
#define PERMUTATOR_HPP

#include <array>
#include "math.hpp"

using wfc::pow;
namespace wfc
{
    template <uint8_t D, uint8_t S> struct Permutator;
    template <uint8_t D, uint8_t S> struct Rotator;
    template <uint8_t D, uint8_t S> struct Reflector;

    template <uint8_t D, uint8_t S>
    struct Permutator
    {
        constexpr virtual std::array<uint8_t, pow(S, D)> getMask() = 0;
    };

    template <uint8_t S>
    struct Rotator<2, S> : Permutator<2, S>
    {
        // Rotates 90° clockwise
        constexpr std::array<uint8_t, pow(S, 2)> getMask() override
        {
            std::array<uint8_t, pow(S, 2)> out = {};

            int i = 0;
            for (int y = 0, y_ = S; y < S; y++, y_--)
            for (int x = 0, x1 = 1; x < S; x++, x1++, i++)
            {
                const uint8_t index = (y_ * x1) - 1 + (y * x);
                out[i] = index;
            }
            return out;
        }
    };

    template <uint8_t S>
    struct Reflector<2, S> : Permutator<2, S>
    {
        // Mirrors on the vertical (Y) axis
        constexpr std::array<uint8_t, pow(S, 2)> getMask() override
        {
            std::array<uint8_t, pow(S, 2)> out = {};

            int i = 0;
            for (int y = 0; y < S; y++)
            for (int x = S; x > 0; x--, i++)
            {
                if (x == pow(S, 2) / 2) continue;

                const uint8_t index = y * S + x - 1;
                out[i] = index;
            }
            return out;
        }
    };
};
#endif // PERMUTATOR_HPP