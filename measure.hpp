#pragma once

#include "data.hpp"
#include "measuring_allocator.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>

template<typename Clock, typename F>
time_unit time(F&& fun) {
    auto start = Clock::now();
    fun();
    return Clock::now() - start;
}

template<typename Map, typename RAIter>
void fill_map(Map& map, RAIter begin, RAIter end) {
    std::for_each(begin, end, [&](auto x) { map.insert(std::make_pair(x, 0)); });
}

template<typename Map, typename Clock, typename RAIter>
time_unit eval_map_insertion(RAIter begin, RAIter end) {
#ifdef SKIP_INSERTION
    return {};
#endif
    time_unit t{};
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        Map ins_map;
        t += time<Clock>([&]() {
            fill_map(ins_map, begin, end);
        });
    }
    return t / REPEAT_COUNT;
}

template<typename Map, typename Clock, typename RAIter>
time_unit eval_map_query(RAIter begin, RAIter end) {
#ifdef SKIP_QUERY
    return {};
#endif
    Map query_map;
    fill_map(query_map, begin, end);
    return time<Clock>([&]() {
        for (int i = 0; i < REPEAT_COUNT; ++i) {
            std::for_each(begin, end, [&](auto x) { query_map.find(x); });
        }
    }) / REPEAT_COUNT;
}

template<typename Map, typename Clock, typename RAIter>
std::size_t eval_map_memory_usage(RAIter begin, RAIter end) {
    reset_allocated();
    Map memory_map;
    fill_map(memory_map, begin, end);
    return get_allocated();
}

template<typename T>
std::set<T> random_subset(std::vector<T> const& vec, std::size_t size) {
    static std::random_device rd;

    // This is inefficient but should work fine.
    if (size > vec.size())
        throw std::runtime_error("not enough data");

    std::set<T> result;
    std::uniform_int_distribution<> dist(0, vec.size()-1);
    while (result.size() < size)
        result.insert(vec[dist(rd)]);

    return result;
}

template<template<template<typename> class> class MapTmpl, typename T, typename OutIter>
std::vector<Result> eval_structure(std::string structure_name, std::string key_type, std::vector<T> const& data, OutIter out) {
    using clock = std::chrono::high_resolution_clock;
    std::vector<Result> results;
    for (std::size_t i = 8; i < 21; ++i) {
        auto const subset = random_subset(data, 1 << i);
        std::size_t const elements = 1 << i;
        using Map = MapTmpl<std::allocator>;
        using MemMeasureMap = MapTmpl<MeasuringAllocator>;
        auto const insert_result = eval_map_insertion<Map, clock>(subset.begin(), subset.end());
        auto const query_result = eval_map_query<Map, clock>(subset.begin(), subset.end());
        auto const memory_usage = eval_map_memory_usage<MemMeasureMap, clock>(subset.begin(), subset.end());
        *out = {structure_name, key_type, elements, insert_result, query_result, memory_usage};
        ++out;
    }
    return results;
}

