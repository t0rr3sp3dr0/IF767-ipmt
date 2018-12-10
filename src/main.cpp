#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include <getopt.h>

#include "dc3.h"
#include "def.h"
#include "lz77.hpp"
#include "lz78.h"
#include "serializer.h"
#include "suffix_array.h"
#include "watch.h"

enum indexing_algorithm {
    SA  = 's',
    DC3 = 'd',
};

enum encoding_algorithm {
    LZ77 = '7',
    LZ78 = '8',
    RAW =  'r',
};

enum mode {
    DEFAULT = 0x00,
    COUNT   = 0x01,
    QUIET   = 0x10,
};

void help_index(char *s) {
    std::cerr
            << "Usage: " << s << " index [OPTION]... FILE" << std::endl
            << "Index FILE." << std::endl
            << "Example: " << s << " index shakespeare.txt" << std::endl
            << std::endl
            << "Pattern selection and interpretation:" << std::endl
            << "  -i, --indexing-algorithm IA    use IA (suffix-array or difference-cover) as indexing algorithm" << std::endl
            << "  -e, --encoding-algorithm EA    use EA (lz77, lz78, or raw) as compression algorithm" << std::endl
            << std::endl
            << "Miscellaneous:" << std::endl
            << "  -h, --help                     display this help text and exit" << std::endl
            << std::endl
            << "Output control:" << std::endl
            << "  -q, --quiet                    suppress all normal output" << std::endl
            ;
}

void help_search(char *s) {
    std::cerr
            << "Usage: " << s << " search [OPTION]... PATTERN FILE" << std::endl
            << "Search for PATTERN in FILE." << std::endl
            << "Example: " << s << " search unworthily shakespeare.idx" << std::endl
            << std::endl
            << "Pattern selection and interpretation:" << std::endl
            << "  -e, --encoding-algorithm EA    use EA (lz77, lz78, or none) as compression algorithm" << std::endl
            << "  -p, --pattern FILE             obtain PATTERN from FILE" << std::endl
            << std::endl
            << "Miscellaneous:" << std::endl
            << "  -h, --help                     display this help text and exit" << std::endl
            << std::endl
            << "Output control:" << std::endl
            << "  -c, --count                    print only a count of selected lines per FILE" << std::endl
            << "  -q, --quiet                    suppress all normal output" << std::endl
            ;
}

void help(char *s) {
    help_index(s);
    std::cerr << std::endl << std::endl;
    help_search(s);
}

int main(int argc, char *argv[]) {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);

    if (argc < 2) {
        help(argv[0]);
        return EXIT_FAILURE;
    }

    switch (argv[1][0]) {
        case 'i': {
            const char *short_options = "i:e:qh";
            const option long_options[] = {
                    {"indexing-algorithm", required_argument, nullptr, 'i'},
                    {"encoding-algorithm", required_argument, nullptr, 'c'},
                    {"quiet",              no_argument,       nullptr, 'q'},
                    {"help",               no_argument,       nullptr, 'h'},
                    {nullptr,              no_argument,       nullptr, '\0'},
            };
            int option_index = -1;

            indexing_algorithm ia = DC3;
            encoding_algorithm ea = LZ77;
            size_t m = mode::DEFAULT;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
                switch (c) {
                    case 'i': {
                        ia = static_cast<indexing_algorithm>(optarg[0]);
                        break;
                    }

                    case 'e': {
                        ea = static_cast<encoding_algorithm>(optarg[0]);
                        break;
                    }

                    case 'q': {
                        m |= mode::QUIET;
                        break;
                    }

                    case 'h':
                    case '?':
                    default: {
                        help_index(argv[0]);
                        return EXIT_FAILURE;
                    }
                }

            switch (ea) {
                case encoding_algorithm::LZ77: {
                    serializer::compressor = [](const ::string_view &sv, std::ostream &os) {
                        lz77::compress(sv, os);
                    };
                    break;
                }

                case encoding_algorithm::LZ78: {
                    serializer::compressor = [](const ::string_view &sv, std::ostream &os) {
                        lz78::compress(sv, os);
                    };
                    break;
                }

                case encoding_algorithm::RAW: {
                    break;
                }

                default: {
                    help_index(argv[0]);
                    return EXIT_FAILURE;
                }
            }

            if (m & mode::QUIET)
                freopen("/dev/null", "w", stderr);

            std::string file_name = argv[++optind];

            std::stringstream ss;
            ss << std::ifstream(file_name).rdbuf();
            std::string s = ss.str();
            ::string_view sv = s;

            std::fstream output(file_name + ".idx", std::ios::out | std::ios::binary | std::ios::trunc);

            switch (ia) {
                case indexing_algorithm::SA: {
                    WATCH(suffix_array sa(sv));
                    WATCH(sa.marshal(output));
                    break;
                }

                case indexing_algorithm::DC3: {
                    WATCH(dc3 sa(sv));
                    WATCH(sa.marshal(output));
                    break;
                }

                default: {
                    help_index(argv[0]);
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }

        case 's': {
            const char *short_options = "e:p:cqh";
            const option long_options[] = {
                    {"encoding-algorithm", required_argument, nullptr, 'e'},
                    {"pattern",            required_argument, nullptr, 'p'},
                    {"count",              no_argument,       nullptr, 'c'},
                    {"quiet",              no_argument,       nullptr, 'q'},
                    {"help",               no_argument,       nullptr, 'h'},
                    {nullptr,              no_argument,       nullptr, '\0'},
            };
            int option_index = -1;

            encoding_algorithm ea = LZ77;
            std::list<std::string> p;
            size_t m = mode::DEFAULT;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
                switch (c) {
                    case 'e': {
                        ea = static_cast<encoding_algorithm>(optarg[0]);
                        break;
                    }

                    case 'p': {
                        std::ifstream ifs(optarg);
                        std::string s;
                        while (ifs >> s)
                            p.emplace_back(s);
                        break;
                    }

                    case 'c': {
                        m |= mode::COUNT;
                        break;
                    }

                    case 'q': {
                        m |= mode::QUIET;
                        break;
                    }

                    case 'h':
                    case '?':
                    default: {
                        help_search(argv[0]);
                        return EXIT_FAILURE;
                    }
                }

            switch (ea) {
                case encoding_algorithm::LZ77: {
                    serializer::decompressor = [](std::istream &is, ::string_view &sv) {
                        lz77::decompress(is, sv);
                    };
                    break;
                }

                case encoding_algorithm::LZ78: {
                    serializer::decompressor = [](std::istream &is, ::string_view &sv) {
                        lz78::decompress(is, sv);
                    };
                    break;
                }

                case encoding_algorithm::RAW: {
                    break;
                }

                default: {
                    help_index(argv[0]);
                    return EXIT_FAILURE;
                }
            }

            if (m & mode::QUIET)
                freopen("/dev/null", "w", stderr);

            if (p.empty())
                p.emplace_back(argv[++optind]);

            std::fstream input(argv[++optind], std::ios::in | std::ios::binary);

            WATCH(suffix_array sa(input));
            if (m & mode::COUNT)
                for (auto &pat : p) {
                    WATCH(size_t count = sa.count(pat));
                    if (!(m & mode::QUIET))
                        std::cout << count << std::endl;
                }
            else
                for (auto &pat : p) {
                    std::vector<size_t> occ;
                    WATCH(sa.find(occ, pat));
                    if (!(m & mode::QUIET))
                        for (auto &pos : occ)
                            std::cout << pos << std::endl;
                }

            return EXIT_SUCCESS;
        }

        default:
            help(argv[0]);
            return EXIT_FAILURE;
    }
}
