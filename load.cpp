#include "load.hpp"
#include <fstream>

std::vector<std::string> read_dataset(std::string const& filename) {
    std::ifstream is(filename);
    std::vector<std::string> result;
    std::string line;
    while (std::getline(is, line))
        result.push_back(line);
    return result;
}
