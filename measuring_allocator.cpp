#include "measuring_allocator.hpp"

std::size_t allocated;

void reset_allocated() {
    allocated = 0;
}

std::size_t get_allocated() {
    return allocated;
}
