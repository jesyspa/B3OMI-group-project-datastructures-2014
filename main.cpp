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

template<typename T>
using skiplist = CS::KeyedSkipList<T, int, std::less<T>, Random>;


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " domains full_paths\n";
        return -1;
    }

    auto data_ips = make_ints(DATA_SIZE);
    auto data_domains = read_lines(std::ifstream(argv[1]));
    auto data_fullpaths = read_lines(std::ifstream(argv[2]));

    print_aligned("structure");
    print_aligned("key");
    print_aligned("entries");
    print_aligned("insert");
    print_aligned("query");
    print_aligned("memory");
    std::cout << std::endl;

    std::ostream_iterator<Result> output(std::cout, "\n");

    eval_structure<skiplist<int>>("skiplist", "ip", data_ips, output);
    eval_structure<skiplist<std::string>>("skiplist", "domain", data_domains, output);
    eval_structure<skiplist<std::string>>("skiplist", "full_path", data_fullpaths, output);
    eval_structure<std::map<int, int>>("bst", "ip", data_ips, output);
    eval_structure<std::map<std::string, int>>("bst", "domain", data_domains, output);
    eval_structure<std::map<std::string, int>>("bst", "full_path", data_fullpaths, output);
    eval_structure<std::unordered_map<int, int>>("hashmap", "ip", data_ips, output);
    eval_structure<std::unordered_map<std::string, int>>("hashmap", "domain", data_domains, output);
    eval_structure<std::unordered_map<std::string, int>>("hashmap", "full_path", data_fullpaths, output);
}
