//
// Created by Pedro Tôrres on 2018-11-30.
//

#include "serializer.h"

#include <fstream>
#include <vector>

void serializer::marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, std::ostream &output) {
    const size_t len = txt.length();
    output.write(reinterpret_cast<const char *>(&len), sizeof(size_t));

    output.write(txt.begin(), len * sizeof(char));
    output.write(reinterpret_cast<const char *>(sa.data()), len * sizeof(size_t));
    output.write(reinterpret_cast<const char *>(l_lcp.data()), len * sizeof(size_t));
    output.write(reinterpret_cast<const char *>(r_lcp.data()), len * sizeof(size_t));
}

void serializer::marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, const std::string &s) {
    std::fstream output(s, std::ios::out | std::ios::trunc | std::ios::binary);
    serializer::marshal(txt, sa, l_lcp, r_lcp, output);
}

char *serializer::unmarshal(std::istream &input, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp) {
    size_t len;
    input.read(reinterpret_cast<char *>(&len), sizeof(size_t));

    char *txt = new char[len + 1];
    input.read(txt, len * sizeof(char));
    txt[len] = '\0';

    sa.resize(len);
    input.read(reinterpret_cast<char *>(sa.data()), len * sizeof(size_t));

    l_lcp.resize(len);
    input.read(reinterpret_cast<char *>(l_lcp.data()), len * sizeof(size_t));

    r_lcp.resize(len);
    input.read(reinterpret_cast<char *>(r_lcp.data()), len * sizeof(size_t));

    return txt;
}

char *serializer::unmarshal(const std::string &s, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp) {
    std::fstream input(s, std::ios::in | std::ios::binary);
    return serializer::unmarshal(input, sa, l_lcp, r_lcp);
}
