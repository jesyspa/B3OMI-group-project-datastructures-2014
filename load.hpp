#pragma once

#include <string>
#include <vector>
#include <iosfwd>

std::vector<int> make_ints(int size);
std::vector<std::string> read_lines(std::istream&& is);
