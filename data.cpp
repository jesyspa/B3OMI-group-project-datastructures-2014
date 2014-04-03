#include "data.hpp"
#include "print.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, Result const& result) {
    print_aligned(result.structure, os);
    print_aligned(result.key_type, os);
    print_aligned(result.elements, os);
    print_aligned(result.insertion_time.count(), os);
    print_aligned(result.query_time.count(), os);
    print_aligned(result.memory_usage, os);
    return os;
}
