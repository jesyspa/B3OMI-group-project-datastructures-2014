#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

int const REPEAT_COUNT = 1'000;
int const DATA_SIZE = 1'000'000;
using time_unit = std::chrono::nanoseconds;

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
        t += time<Clock>([&]() {
            Map map;
            std::for_each(begin, end, [&](auto x) { map.insert(std::make_pair(x, x)); });
        });
    }
    os << t.count()/REPEAT_COUNT << "\n";
}

int main() {
    std::vector<int> data;
    for (int i = 0; i < DATA_SIZE; ++i)
        data.push_back(i);
    eval_map<std::map<int, int>, std::chrono::high_resolution_clock>(std::cout, data.begin(), data.end());
}
