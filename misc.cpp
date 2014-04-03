#include "misc.hpp"

#include <proc/readproc.h>
#include <stdexcept>

long get_memory_usage() {
    proc_t usage;
    look_up_our_self(&usage);
    return usage.size;
}
