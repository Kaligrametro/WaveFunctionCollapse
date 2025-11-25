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
#include <utility>

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

enum class STRATEGY {
    MIN_ENTROPY,
    RANDOM
};

template <
	typename T, // T: datatype
	uint8_t D,  // D: dimension
	size_t S,   // S: pattern size
    size_t N    // N: number of patterns
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

    struct Element {
        Element() : 
            _entropy(1.0f), 
            _isCollapsed(false)
        {}

        void collapse(const T& value) {
            this->_value       = value;
            this->_entropy     = 0.0f;
            this->_isCollapsed = true;
        }

        float  getEntropy()  const { return this->_entropy;     }
        T      getValue()    const { return this->_value;       }
        bool   isCollapsed() const { return this->_isCollapsed; }

    private:
        float  _entropy;
        bool   _isCollapsed;
        T      _value;
    };
    
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

    struct Wave {

        Wave(const Vector size, WFC& wfc) : 
            _elements(size.size()),
            _size(size),
            _wfc(wfc)
        {}

        Element& at(const Vector position) 
        { 
            size_t index = -1;
            switch(D)
            {
                case 2: index = position.x + position.y * S; 
                case 3: index = position.x + position.y * S + position.z * S * S;
            }
            return _elements.at(index); 
        }

        void run(std::function<void(const Wave&, int)> callback = [](const Wave&, int) {})
        {
            callback(*this, -1);

            for (size_t collapsed = 0; collapsed < _elements.size(); collapsed++)
            {

                size_t  index   = searchElement(STRATEGY::MIN_ENTROPY);
                Pattern pattern = searchPattern(STRATEGY::RANDOM);

                collapse(pattern.value, index);
                propagate(index);

                callback(*this, (int) collapsed);
            }
        }

        std::vector<Element> getElements() const { return _elements; }
        Vector getSize() const { return _size; }
    
    private:
        WFC& _wfc;
        std::vector<Element> _elements;
        Vector _size;

        void propagate(const size_t lastCollapsedIndex) 
        {
            //const T& value = this->at(lastCollapsedIndex).getValue();
            //const auto patterns = wfc::utils::filter<Pattern>(_wfc.patterns, [&](const Pattern& p) { return p.value == value; });
            // TODO
        }

        // returns index of element with specific search strategy
        size_t searchElement(STRATEGY strategy)
        {
            size_t toCollapseIndex = 0;

            switch (strategy)
            {
                case STRATEGY::MIN_ENTROPY:
                {
                    /*
                    collapsedIndex = (size_t) std::distance(
                        _elements.begin(),
                        std::min_element(
                            _elements.begin(),
                            _elements.end(),
                            [](const Element& a, const Element& b) { 
                                if (a.isCollapsed()) return true; 
                                return a.getEntropy() < b.getEntropy(); 
                            }
                        )
                    );
                    */

                    // find first non-collapsed element
                    const auto uncollapsed = std::find_if(_elements.begin(), _elements.end(), [](const Element& e) { return !e.isCollapsed(); });

                    if (uncollapsed == _elements.end())
                        return _elements.size(); // all collapsed

                    toCollapseIndex = (size_t) std::distance(_elements.begin(), uncollapsed);
                    println(toCollapseIndex);

                    for (size_t i = 0; i < _elements.size(); i++)
                    {
                        if (_elements.at(i).isCollapsed()) continue;
                        if (_elements.at(i).getEntropy() < _elements.at(toCollapseIndex).getEntropy())
                        {
                            toCollapseIndex = i;
                        }
                    }
                }
                break;
                case STRATEGY::RANDOM:
                {
                    toCollapseIndex = wfc::utils::random(0, _elements.size() - 1);
                }
                break;
            }

            return toCollapseIndex;
        }

        ;
        // returns hash of pattern with specific search strategy
        Pattern searchPattern(STRATEGY strategy) 
        {
            return _wfc.patterns[0]; // TODO: implement
        }

        void collapse(const T& value, size_t collapseIndex)
        {
            _elements
            .at(collapseIndex)
            .collapse(value);
        }
    };

    // Generate all possible patterns
    std::vector<Pattern> patternGeneration(
        const std::vector<T>& baseImage, 
        const Vector&         size)
    {
        using Patterns = std::vector<Pattern>;

        // Grab patterns from base image TODO
        auto generatePatterns = [&](const std::vector<T>& baseImage, const Vector& size) -> Patterns 
        {
			auto inBounds = [](Vector size, int x, int y) -> bool
			{
				return (x + ((int) S) < size.x && y + ((int) S) < size.y);
			};

			Patterns out;

            // iterate through base image
			for (int y = 0; y < size.y; y += S - 1) {
			for (int x = 0; x < size.x; x += S - 1) {

					if (!inBounds(size, x, y)) continue;

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

    std::vector<Pattern> patterns;
public:

    WFC(const std::vector<T>& baseImage, 
        const Vector size)
    {
        this->patterns = patternGeneration(baseImage, size);
        println(this->patterns.size());
    }

    // steps:
    // 1. initialize wave function
    // 2. observe
    // 3. propagate
    void run(const Vector outputSize)
    {
        const auto callback = [](const Wave& w, int frame) {
            exportFrame(w.getElements(), w.getSize(), frame);
        };


        Wave wave(outputSize, *this);
        wave.run(callback);
    }

#if DEBUG
    static void exportFrame(const std::vector<Element>& data, const Vector& size, const int frameNumber)
    {
        std::string path = "output/frames/frame(" + std::to_string(frameNumber) + ").bmp";
        bmp::Bitmap file(size.x, size.y);

        std::vector<T> castedData;
        for (const Element& e : data)
        {
            castedData.push_back(e.getValue());
        }
        file.setCastPixels(castedData);

        file.save(path);
    }

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
    static void exportFrame(const std::vector<T>& data, const Vector& size, const int frameNumber) {}
#endif
};

#endif // WFC_HPP