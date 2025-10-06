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
#include <optional>
#include <vector>

struct Pattern { size_t hash() const; };
struct Element;
struct Wave;

// hashables
template<> struct std::hash<Pattern> {
    std::size_t operator()(const Pattern& p) const noexcept { return p.hash(); }
};

//template <uint8_t D> constexpr size_t pow(const size_t s, const size_t d);
template <uint8_t D> struct Vector;
template <uint8_t D, uint8_t S> struct Rotator;
template <uint8_t D, uint8_t S> struct Reflector;

constexpr size_t pow(size_t s, size_t d) 
{
    for (size_t i = 1; i < d; i++) s *= s;
    return s;
}

// dimensional operators
// 2D

template <> struct Vector<2> {
    Vector() : x(0), y(0) {}
    Vector(const int x_, const int y_) : x(x_), y(y_) {}
    int x, y;
};

template <uint8_t S> struct Rotator<2, S> 
{
    static constexpr size_t VOL = pow(S, 2);

    // returns a 90° rotation position mapping matrix for SxS patterns
    static constexpr std::array<std::array<uint8_t, VOL>, 3> RotationMatrixes()
    {
        std::array<uint8_t, VOL> rotationMatrix = {}, tmp = {};
        int i = 0;
        for (int y = 0, y_ = S; y < S; y++, y_--) 
        for (int x = 0, x1 = 1; x < S; x++, x1++) 
        {
            rotationMatrix[i] = (y_ * x1) - 1 + (y * x);
            i++;
        }

        // fill with nums
        tmp = rotationMatrix;

        auto rotate = [rotationMatrix](std::array<uint8_t, VOL> base) 
        {
            for (size_t i = 0; i < VOL; i++) 
                base[i] = base[rotationMatrix[i]];
            return base;
        };

        return {
            rotationMatrix,
            rotate(tmp),
            rotate(rotate(tmp))
        };
    };
};

template <uint8_t S> struct Reflector<2, S> 
{
    static constexpr size_t VOL = pow(S, 2);

    static constexpr std::array<std::array<uint8_t, VOL>, 3> ReflectorMatrixes()
    {
        std::array<uint8_t, VOL> mirrorX  = {};
        std::array<uint8_t, VOL> mirrorY  = {};
        std::array<uint8_t, VOL> mirrorXY = {};
        
        int i = 0;
		for (int y = 0; y < S; y++) 
		for (int x = S; x > 0; x--, i++) 
            mirrorX[i] = y * S + x - 1;
		
        i = 0;
		for (int y = S; y > 0; y--) 
		for (int x = 0; x < S; x++, i++) 
            mirrorY[i] = (y - 1) * S + x;

        i = 0;
        // TODO: add mirrorXY

        return { mirrorX, mirrorY, mirrorXY };
    }
}; 

// 3D
/* ... */

template <
	typename T, // T: datatype
	uint8_t D,  // D: dimension
	size_t S    // S: pattern size
> 
class WFC
{
    static constexpr size_t VOL = pow(S, D);

    using Vector = Vector<D>;
    using Rotator = Rotator<D, S>;
    using Reflector = Reflector<D, S>;

    struct Element {};
    
    struct Pattern {
        
        std::array<T, VOL> data;
        T& value;
        float frequency;
        
        [[nodiscard]] inline size_t hash() const
		{
			if (_hash.has_value())
				return _hash.value();

			// random shit go!
			size_t v = 0x89abcdef;
			for (const T& data : this->data)
			{
				v ^= data + 0x1234 + (v >> 4) + (v << 2);
			}
			_hash = v;
			return _hash.value();
		}
    
    private:
        std::optional<size_t> _hash = std::nullopt;
    };
    
    std::vector<Pattern> patterns;

public:

    WFC(const std::vector<T>& baseImage, const Vector size) 
    {
        auto permutate = [](const Pattern& pattern) 
        {
            for (const auto& rot : Rotator::RotationMatrixes())
            {
                // apply rotation
                
            }
        };
        //auto deduplicate = [](const Pattern& patterns) -> std::vector<Pattern> {};
    }
};

#endif // WFC_HPP