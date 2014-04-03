#include "load.hpp"

#include <algorithm>
#include <iostream>

std::vector<int> make_ints(int size) {
    std::vector<int> vec(size);
    std::iota(vec.begin(), vec.end(), 0);
    return vec;
}

std::vector<std::string> read_lines(std::istream&& is) {
    std::vector<std::string> vec;
    std::string line;
    while (std::getline(is, line))
        vec.push_back(line);
    return vec;
}

