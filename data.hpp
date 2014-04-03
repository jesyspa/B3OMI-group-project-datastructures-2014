#pragma once

#include "types.hpp"

#include <string>
#include <iosfwd>

struct Result {
    std::string structure;
    std::string key_type;
    std::size_t elements;
    time_unit insertion_time;
    time_unit query_time;
    std::size_t memory_usage;
};

std::ostream& operator<<(std::ostream&, Result const&);
