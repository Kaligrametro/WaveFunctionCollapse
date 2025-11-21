#ifndef WFC_HPP
#define WFC_HPP

#define DEBUG true
#if DEBUG
    #include <iostream>
    #include "BMP_reader.hpp"
    #define println(x) std::cout << #x << ": " << x << '\n'
#else
    #define println(x)
#endif

#include <array>
#include <optional>
#include <vector>
#include <algorithm>

#include "Permutator.hpp"
#include "Vector.hpp"
#include "math.hpp"
#include "utils.hpp"

using wfc::pow;
using wfc::Permutator;
using wfc::Rotator;
using wfc::Reflector;

struct Element;
struct Wave;
struct Pattern;

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
    public: 
        std::array<T, VOL> data;
        T& value;
        float frequency;

        //ctors
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

        // operators
        Pattern& operator=(const Pattern& other)
        {
            this->data      = other.data;
            this->frequency = other.frequency;
            return *this;
        }
        
        bool     operator==(const Pattern& other) const
        {
            return (hash() == other.hash());
        }

        [[nodiscard]] size_t hash() const
		{
			if (this->_hash.has_value())
				return this->_hash.value();

			// random shit go!
			size_t v = 0x89abcdef;
			for (size_t i = 0; i < this->data.size(); i++)
			{
				v ^= this->data[i] + 0x1234 + (v >> 4) + (v << 2);
			}
			this->_hash = v;
			return this->_hash.value();
		}

    private:
        mutable std::optional<size_t> _hash;
    };

    std::vector<Pattern> patterns;

    // Generate all possible patterns
    std::vector<Pattern> patternGeneration(
        const std::vector<T>& baseImage, 
        const Vector&         size)
    {
        using Patterns = std::vector<Pattern>;

        // Grab patterns from base image TODO
        auto generatePatterns = [&](const std::vector<T>& baseImage, const Vector& size) -> Patterns 
        {
			auto inBounds = [size](int x, int y) -> bool
			{
				return (x + ((int) S) < size.x && y + ((int) S) < size.y);
			};

			Patterns out;

            // iterate through base image
			for (int y = 0; y < size.y; y += S - 1) {
			for (int x = 0; x < size.x; x += S - 1) {

					if (!inBounds(x, y)) continue;

					Pattern	tmp;

					for (int _y = 0; _y < S; _y++) {
					for (int _x = 0; _x < S; _x++) {
						int data_index = (x + _x) + ((y + _y) * size.x);
						int copy_index = _x + _y * S;
						tmp.data[copy_index] = baseImage[data_index];
					}
					}

					out.push_back(tmp);
				}
			}
			return out;
        };

        // Generate permutations (includes original)
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

            Patterns out;
            Rotator rotator;
            Reflector reflector;
            
            switch (D)
            {
                case 2:
                    out.resize(8); // 4 rotations + 4 reflections
                    out[0] = pattern;                               // 0°
                    out[1] = applyPermutation(out[0],  &rotator);   // 90°
                    out[2] = applyPermutation(out[1],  &rotator);   // 180°
                    out[3] = applyPermutation(out[2],  &rotator);   // 270°
                    out[4] = applyPermutation(out[0],  &reflector); // mirror Y 0°
                    out[5] = applyPermutation(out[1],  &reflector); // mirror Y 90°
                    out[6] = applyPermutation(out[2],  &reflector); // mirror Y 180°
                    out[7] = applyPermutation(out[3],  &reflector); // mirror Y 270°
                    break;
                case 3:
                    // TODO: implement for D == 3
                    static_assert(D == 2, "Permutations are only implemented for 2D patterns.");
                    break;
            }

            return out;
        };
        
        // Apply permutations to all patterns
        auto permuteAllPatterns = [&](const Patterns& patterns) -> Patterns 
        {
            Patterns out;
            for (const Pattern& pattern : patterns)
            {
                const auto permutations = permutePattern(pattern);
                out.insert(out.end(), permutations.begin(), permutations.end());  // append permutations
            }
            return out;
        };
        
        // Deduplicate patterns and calculate frequencies   //TODO:FIX
        auto deduplicate = [&](Patterns patterns) -> Patterns
        {
            Patterns out;
            const float original_size = (float) patterns.size();
            
            for (const Pattern& pattern : patterns)
            {
                Patterns filtered = wfc::utils::filter<Pattern>(
                    patterns, [&](const Pattern& p) { return p == pattern; }
                );
                
                // check if already exists in out
                if (std::find(out.begin(), out.end(), filtered[0]) != out.end())
                    continue;

                filtered[0].frequency = (float) (filtered.size() / original_size);
                out.push_back(filtered[0]); 
            }
            
            return out;
        };
        
        return deduplicate( permuteAllPatterns( generatePatterns(baseImage, size) ) );
    }


public:

    WFC(const std::vector<T>& baseImage, 
        const Vector size) 
    {
        this->patterns = patternGeneration(baseImage, size);

        println(this->patterns.size());

    }

#if DEBUG
    static void export_patterns(const std::vector<Pattern>& patterns)
	{
		bmp::Bitmap file(S, S);
		std::string filename;
		for (size_t i = 0; i < patterns.size(); i++)
		{
			auto data = patterns[i].data;
			file.setCastPixels(std::vector<T>(data.begin(), data.end()));
			filename = "output/patterns/pattern(" + std::to_string(i) + ").bmp";
			file.save(filename);
		}
		std::cout << "<DEBUG-ONLY> Patterns(" << patterns.size() << ") exported succesfully!\n";
    }
#else
    static void export_patterns(const std::vector<Pattern>& patterns) {}
#endif
};

#endif // WFC_HPP