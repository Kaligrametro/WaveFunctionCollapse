#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <functional>
#include <random>

namespace wfc::utils
{

// vector filtering by lambda
template <typename T>
[[nodiscard]]
std::vector<T> filter(
    const std::vector<T> &data,
    std::function<bool(const T &)> condition)
{
    std::vector<T> out;
    for (int i = 0; i < data.size(); i++)
    {
        const T &item = data[i];
        if (condition(item)) out.push_back(item);
    }
    return out;
}


int random(int min, int max) {
    static std::random_device rd; // Obtain a random number from hardware
    static std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distrib(min, max); // Define a distribution for min to max

    return distrib(gen); // Generate a random number
}

};

#endif // UTILS_HPP