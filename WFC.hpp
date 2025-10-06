#ifndef WFC_HPP
#define WFC_HPP

#define DEBUG false
#if DEBUG
    #include <iostream>
    #define println(x) std::cout << #x << ": " << x << '\n'
#else
    #define println(x)
#endif

#include <array>

struct Pattern;
struct Element;
struct Wave;

template <uint8_t D> struct Vector;
template <uint8_t D, uint8_t S> struct Rotator;
template <uint8_t D, uint8_t S> struct Reflector;

constexpr size_t pow(size_t s, size_t d) {
    for (size_t i = 1; i < d; i++) s *= s;
    return s;
}

// dimensional operators
// 2D
template <> struct Vector<2> {};

template <uint8_t S> struct Rotator<2, S> 
{
    // returns a 90° rotation position mapping matrix for SxS patterns
    static constexpr std::array<uint8_t, VOL> RotationMatrix()
    {
        std::array<uint8_t, VOL> out = {};
        int c = 0;
        for (int y = 0, y_ = S; y < S; y++, y_--) 
        for (int x = 0, x1 = 1; x < S; x++, x1++) 
        {
            int index = (y_ * x1) - 1 + (y * x);
            out[c] = index;
            c++;
        }
        return out;
    };

};

template <uint8_t S> struct Reflector<2, S> {}; 

// 3D
/* ... */

template <
	typename T, // T: datatype
	uint8_t D,  // D: dimension
	size_t S    // S: pattern size
> 
class WFC
{
   

    
    struct Element {};
    struct Pattern {
        std::array<T, VOL> data;
        T center 
        size_t hash() const {}
    };

public:
    WFC(){
        println("WFC::WFC()");
    }
    WFC(
        //T baseImage,
        size_t width, size_t height) {

    }
};

#endif // WFC_HPP