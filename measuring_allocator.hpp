#pragma once

#include <memory>

template<typename T>
struct MeasuringAllocator : std::allocator<T> {
    template<typename U>
    struct rebind {
        using other = MeasuringAllocator<U>;
    };

    MeasuringAllocator() = default;
    MeasuringAllocator(MeasuringAllocator const&) = default;
    template<typename U>
    MeasuringAllocator(MeasuringAllocator<U> const& other) : base(other) {}

    MeasuringAllocator& operator=(MeasuringAllocator const&) = default;
    template<typename U>
    MeasuringAllocator& operator=(MeasuringAllocator<U> const& other) {
        base::operator=(other);
        return *this;
    }

    using base = std::allocator<T>;
    typename base::pointer allocate(typename base::size_type n, std::allocator<void>::const_pointer hint = 0) {
        extern std::size_t allocated;
        allocated += n * sizeof(T);
        return base::allocate(n, hint);
    }

};

void reset_allocated();
std::size_t get_allocated();

