#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "dc3.h"
#include "def.h"
#include "lz77.hpp"
#include "suffix_array.h"
#include "watch.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3)
        return EXIT_FAILURE;

    if (argc == 2) {
        auto idx = std::string(argv[1]) + ".idx";
        auto txt = (std::stringstream() << std::ifstream(argv[1]).rdbuf()).str();

        WATCH(dc3 sa(txt));

        std::fstream output(idx, std::ios::out | std::ios::binary | std::ios::trunc);
        WATCH(sa.marshal(output));
    } else {
        auto idx = argv[1];
        auto pat = argv[2];

        std::fstream input(idx, std::ios::in | std::ios::binary);
        WATCH(suffix_array sa(input));

//        std::vector<size_t> occ;
//        sa.find(occ, pat);
//
//        for (auto &e : occ)
//            std::cout << e << ' ';
//        std::cout << std::endl;

        WATCH(size_t count = sa.count(pat));
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
