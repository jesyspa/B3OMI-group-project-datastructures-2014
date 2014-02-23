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

template<typename T>
void print_aligned(T&& t, std::ostream& os = std::cout) {
    os << std::setw(16) << std::forward<T>(t);
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
    print_aligned(t.count()/REPEAT_COUNT);
    Map query_map;
    std::for_each(begin, end, [&](auto x) { query_map.insert(std::make_pair(x, x)); });
    t += time<Clock>([&]() {
        for (int i = 0; i < REPEAT_COUNT; ++i) {
            std::for_each(begin, end, [&](auto x) { query_map.find(x); });
        }
    });
    print_aligned(t.count()/REPEAT_COUNT);
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
    print_aligned("structure");
    print_aligned("entries");
    print_aligned("key_complexity");
    print_aligned("insert");
    print_aligned("query");
    std::cout << std::endl;
    for (int i = 8; i < 20; ++i) {
        auto subset = random_subset(data, 1 << i);
        print_aligned("bst");
        print_aligned(1 << i);
        print_aligned("int");
        eval_map<std::map<int, int>, std::chrono::high_resolution_clock>(std::cout, subset.begin(), subset.end());
    }
    for (int i = 8; i < 20; ++i) {
        auto subset = random_subset(data, 1 << i);
        print_aligned("hashmap");
        print_aligned(1 << i);
        print_aligned("int");
        eval_map<std::unordered_map<int, int>, std::chrono::high_resolution_clock>(std::cout, subset.begin(), subset.end());
    }
}
