#ifndef WFC_HPP
#define WFC_HPP

#define DEBUG true
#if DEBUG
    #include <iostream>
    #define println(x) std::cout << #x << ": " << x << '\n'
#else
    #define println(x)
#endif

#include <array>
#include <optional>
#include <vector>

#include "Permutator.hpp"
#include "Vector.hpp"
#include "math.hpp"

using wfc::pow;
using wfc::Permutator;
using wfc::Rotator;
using wfc::Reflector;

struct Pattern { size_t hash() const; };
struct Element;
struct Wave;

// hashables
template<> struct std::hash<Pattern> 
{
    std::size_t operator()(const Pattern& p) const noexcept { return p.hash(); }
};

template <
	typename T, // T: datatype
	uint8_t D,  // D: dimension
	size_t S    // S: pattern size
> 
requires (
    (D == 2 || D == 3) &&   // 2D or 3D only
    (S % 2 == 1)       &&   // S must be odd
    (S > 0 && S <= 15)      // S must cannot exceed 15 ( 15*15 = 255 = sizeof(uint8_t) )
) 
class WFC
{
    static constexpr size_t VOL = pow(S, D);
    
    using Vector     = wfc::Vector     <D>;
    using Permutator = wfc::Permutator <D, S>;
    using Rotator    = wfc::Rotator    <D, S>;
    using Reflector  = wfc::Reflector  <D, S>;

    struct Element {};
    
    struct Pattern {
        
        std::array<T, VOL> data;
        T& value;
        float frequency;

        Pattern() : 
            data{}, 
            value(data[VOL/2]), 
            frequency(0.0f) 
        {}

        Pattern(const std::array<T, VOL>& data) : 
            data(data), 
            value(data[VOL/2]), 
            frequency(0.0f) 
        {}

        Pattern& operator=(const Pattern& other)
        {
            this->data      = other.data;
            this->frequency = other.frequency;
            return *this;
        }
        
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

    // Generate all possible patterns
    std::vector<Pattern> patternGeneration(const Vector& pos)
    {
        using Patterns = std::vector<Pattern>;
        Patterns patterns;

        // Initialize patterns from base image
        auto generatePatterns = [&](const Vector& pos) -> Patterns 
        {
            return {};
        };

        // Generate permutations
        auto permutePattern = [&](const Pattern& pattern) -> Patterns 
        {            
            auto applyPermutation = [&](const Pattern& pattern, Permutator*    permutator) -> Pattern 
            {
                Pattern out;
                auto mask = permutator->getMask();

                for (size_t i = 0; i < VOL; i++) 
                    out.data[i] = pattern.data[mask[i]];

                return out;
            };

            // TODO: implement for D == 3
            static_assert(D == 2, "Permutations are only implemented for 2D patterns.");

            Patterns out(8);
            Rotator rotator;
            Reflector reflector;
            
            // this amount of permutations is enough to generate all unique rotations/reflections, for reference see ./devguides/PermutationsReduction.png

            out[0] = pattern;                               // 0°
            out[1] = applyPermutation(out[0],  &rotator);   // 90°
            out[2] = applyPermutation(out[1],  &rotator);   // 180°
            out[3] = applyPermutation(out[2],  &rotator);   // 270°

            out[4] = applyPermutation(out[0],  &reflector); // mirror Y 0°
            out[5] = applyPermutation(out[1],  &reflector); // mirror Y 90°
            out[6] = applyPermutation(out[2],  &reflector); // mirror Y 180°
            out[7] = applyPermutation(out[3],  &reflector); // mirror Y 270°

            return out;
        };
    
        return {};
    }

public:

    WFC(const std::vector<T>& baseImage, 
        const Vector size) 
    {
       this->patterns = patternGeneration(size);

    }
};

#endif // WFC_HPP