#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

int const REPEAT_COUNT = 1'000;
int const DATA_SIZE = 1'000'000;
using time_unit = std::chrono::nanoseconds;

// TODO: Use a predicatable RNG.
std::random_device rd;

template<typename Clock, typename F>
time_unit time(F&& fun) {
    auto start = Clock::now();
    fun();
    return Clock::now() - start;
}

template<typename Map, typename Clock, typename RAIter>
void eval_map(std::ostream& os, RAIter begin, RAIter end) {
    time_unit t;
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        Map ins_map;
        t += time<Clock>([&]() {
            std::for_each(begin, end, [&](auto x) { ins_map.insert(std::make_pair(x, x)); });
        });
    }
    os << std::setw(12) << t.count()/REPEAT_COUNT << '\t';
    Map query_map;
    std::for_each(begin, end, [&](auto x) { query_map.insert(std::make_pair(x, x)); });
    t += time<Clock>([&]() {
        for (int i = 0; i < REPEAT_COUNT; ++i) {
            std::for_each(begin, end, [&](auto x) { query_map.find(x); });
        }
    });
    os << std::setw(12) << t.count()/REPEAT_COUNT << '\t';
    os << '\n';
}

template<typename T>
std::vector<T> random_subset(std::vector<T>& vec, std::size_t size) {
    // This is inefficient but should work fine.
    if (size > vec.size())
        throw std::runtime_error("not enough data");
    std::vector<T> result = vec;
    std::shuffle(result.begin(), result.end(), rd);
    result.resize(size);
    return result;
}

int main() {
    std::vector<int> data;
    for (int i = 0; i < DATA_SIZE; ++i)
        data.push_back(i);
    std::cout << std::setw(12) << "insert";
    std::cout << std::setw(12) << "query";
    std::cout << std::endl;
    eval_map<std::map<int, int>, std::chrono::high_resolution_clock>(std::cout, data.begin(), data.end());
}
