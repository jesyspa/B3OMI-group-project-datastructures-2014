#pragma once

#include <iostream>
#include <iomanip>

template<typename T>
void print_aligned(T&& t, std::ostream& os = std::cout) {
    os << std::setw(16) << std::forward<T>(t);
}

