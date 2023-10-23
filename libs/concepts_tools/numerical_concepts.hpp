#ifndef NUMERICAL_CONCEPTS_HPP
#define NUMERICAL_CONCEPTS_HPP

#include <concepts>

namespace ocv::concepts{

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

}

#endif // NUMERICAL_CONCEPTS_HPP
