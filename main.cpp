#include "load.hpp"
#include "measure.hpp"
#include "print.hpp"

#include "CSKeyedSkipList.h"

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <random>
#include <unordered_map>
#include <iterator>

int const DATA_SIZE = 1'050'000;

// We define our own random.
struct Random {
    static std::mt19937 engine;

    unsigned int rand() const {
        return engine();
    }

    double drand() const {
        static std::uniform_real_distribution<double> dist;
        return dist(engine);
    }
};

std::mt19937 Random::engine(std::random_device{}());

template<template<typename...> class Underlying, typename T>
struct Map;

template<typename T>
struct Map<std::map, T> {
    using value_type = std::pair<const T, int>;
    template<template<typename> class Alloc>
    using type = std::map<T, int, std::less<T>, Alloc<value_type>>;
};

template<typename T>
struct Map<std::unordered_map, T> {
    using value_type = std::pair<const T, int>;
    template<template<typename> class Alloc>
    using type = std::unordered_map<T, int, std::hash<T>, std::equal_to<T>, Alloc<value_type>>;
};

template<typename T>
struct Map<CS::KeyedSkipList, T> {
    using value_type = std::pair<const T, int>;
    template<template<typename> class Alloc>
    using type = CS::KeyedSkipList<T, int, std::less<T>, Random, Alloc<value_type>>;
};

int main(int argc, char* argv[]) {
    std::cout.sync_with_stdio(false);
    std::cin.sync_with_stdio(false);
    std::cerr.sync_with_stdio(false);
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " domains full_paths\n";
        return -1;
    }

    auto data_ips = make_ints(DATA_SIZE);
    auto data_domains = read_lines(std::ifstream(argv[1]));
    auto data_fullpaths = read_lines(std::ifstream(argv[2]));

    print_aligned("MapType");
    std::cout << ',';
    print_aligned("KeyType");
    std::cout << ',';
    print_aligned("Entries");
    std::cout << ',';
    print_aligned("Insert");
    std::cout << ',';
    print_aligned("Query");
    std::cout << ',';
    print_aligned("Memory");
    std::cout << std::endl;

    std::ostream_iterator<Result> output(std::cout, "\n");

#ifndef NO_SKIPLIST
    eval_structure<Map<CS::KeyedSkipList, int>::type>("skiplist", "ip", data_ips, output);
    eval_structure<Map<CS::KeyedSkipList, std::string>::type>("skiplist", "domain", data_domains, output);
    eval_structure<Map<CS::KeyedSkipList, std::string>::type>("skiplist", "full_path", data_fullpaths, output);
#endif
#ifndef NO_BST
    eval_structure<Map<std::map, int>::type>("bst", "ip", data_ips, output);
    eval_structure<Map<std::map, std::string>::type>("bst", "domain", data_domains, output);
    eval_structure<Map<std::map, std::string>::type>("bst", "full_path", data_fullpaths, output);
#endif
#ifndef NO_HASHMAP
    eval_structure<Map<std::unordered_map, int>::type>("hashmap", "ip", data_ips, output);
    eval_structure<Map<std::unordered_map, std::string>::type>("hashmap", "domain", data_domains, output);
    eval_structure<Map<std::unordered_map, std::string>::type>("hashmap", "full_path", data_fullpaths, output);
#endif
}
