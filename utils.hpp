#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <functional>

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

};

#endif // UTILS_HPP