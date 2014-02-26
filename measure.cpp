#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <set>

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

template<typename Map, typename RAIter>
void fill_map(Map& map, RAIter begin, RAIter end) {
    std::for_each(begin, end, [&](auto x) { map.insert(std::make_pair(x, 0)); });
}

template<typename Map, typename Clock, typename RAIter>
void eval_map_insertion(std::ostream& os, RAIter begin, RAIter end) {
#ifdef SKIP_INSERTION_MEASURE
    return;
#endif
    time_unit t{};
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        Map ins_map;
        t += time<Clock>([&]() {
            fill_map(ins_map, begin, end);
        });
    }
    print_aligned(t.count()/REPEAT_COUNT, os);
}

template<typename Map, typename Clock, typename RAIter>
void eval_map_query(std::ostream& os, RAIter begin, RAIter end) {
#ifdef SKIP_QUERY_MEASURE
    return;
#endif
    time_unit t{};
    Map query_map;
    fill_map(query_map, begin, end);
    t += time<Clock>([&]() {
        for (int i = 0; i < REPEAT_COUNT; ++i) {
            std::for_each(begin, end, [&](auto x) { query_map.find(x); });
        }
    });
    print_aligned(t.count()/REPEAT_COUNT, os);
}

template<typename Map, typename Clock, typename RAIter>
void eval_map(std::ostream& os, RAIter begin, RAIter end) {
    eval_map_insertion<Map, Clock>(os, begin, end);
    eval_map_query<Map, Clock>(os, begin, end);
    os << '\n';
}

template<typename T>
std::set<T> random_subset(std::vector<T> const& vec, std::size_t size) {
    // This is inefficient but should work fine.
    if (size > vec.size())
        throw std::runtime_error("not enough data");

    std::set<T> result;
    std::uniform_int_distribution<> dist(0, vec.size()-1);
    while (result.size() < size)
        result.insert(vec[dist(rd)]);

    return result;
}

template<typename Map, typename T>
void eval_structure(std::ostream& os, std::string const& structure_name, std::string const& data_size, std::vector<T> const& data) {
    for (int i = 8; i < 20; ++i) {
        auto subset = random_subset(data, 1 << i);
        print_aligned(structure_name, os);
        print_aligned(1 << i);
        print_aligned(data_size);
        eval_map<Map, std::chrono::high_resolution_clock>(os, subset.begin(), subset.end());
    }
}

std::vector<int> make_ints(int size) {
    std::vector<int> vec(size);
    std::iota(vec.begin(), vec.end(), 0);
    return vec;
}

std::vector<std::string> read_lines(std::istream&& is) {
    std::vector<std::string> vec;
    std::string line;
    while (std::getline(is, line))
        vec.push_back(line);
    return vec;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " domains full_paths\n";
        return -1;
    }

    auto data_ips = make_ints(DATA_SIZE);
    auto data_domains = read_lines(std::ifstream(argv[1]));
    auto data_fullpaths = read_lines(std::ifstream(argv[2]));

    print_aligned("structure");
    print_aligned("entries");
    print_aligned("key_complexity");
#ifndef SKIP_INSERTION_MEASURE
    print_aligned("insert");
#endif
#ifndef SKIP_QUERY_MEASURE
    print_aligned("query");
#endif
    std::cout << std::endl;

    eval_structure<std::map<int, int>>(std::cout, "bst", "ip", data_ips);
    eval_structure<std::map<std::string, int>>(std::cout, "bst", "domain", data_domains);
    eval_structure<std::map<std::string, int>>(std::cout, "bst", "full_path", data_fullpaths);
    eval_structure<std::unordered_map<int, int>>(std::cout, "hashmap", "ip", data_ips);
    eval_structure<std::unordered_map<std::string, int>>(std::cout, "hashmap", "domain", data_domains);
    eval_structure<std::unordered_map<std::string, int>>(std::cout, "hashmap", "full_path", data_fullpaths);
}
