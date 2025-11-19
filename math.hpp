#ifndef MATH_HPP
#define MATH_HPP
namespace wfc {
    constexpr size_t pow(size_t s, const size_t d) 
    {
        for (size_t i = 1; i < d; i++) s *= s;
        return s;
    }
};

#endif // MATH_HPP