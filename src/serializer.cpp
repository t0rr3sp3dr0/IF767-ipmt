//
// Created by Pedro Tôrres on 2018-11-30.
//

#include "serializer.h"

#include <fstream>
#include <vector>

#include "lz77.hpp"
#include "watch.h"

static constexpr size_t MULTIPLIER = sizeof(size_t) / sizeof(char);

void serializer::marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, std::ostream &output) {
    const size_t len = txt.length();
    output.write(reinterpret_cast<const char *>(&len), sizeof(size_t));

    WATCH(lz77::compress(txt, output));

    WATCH(output.write(reinterpret_cast<const char *>(sa.data()), len * MULTIPLIER));

    ::string_view l_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<const char *>(l_lcp.data()), len * MULTIPLIER);
    WATCH(lz77::compress(l_lcp_sv, output));

    ::string_view r_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<const char *>(r_lcp.data()), len * MULTIPLIER);
    WATCH(lz77::compress(r_lcp_sv, output));
}

void serializer::marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, const std::string &s) {
    std::fstream output(s, std::ios::out | std::ios::trunc | std::ios::binary);
    serializer::marshal(txt, sa, l_lcp, r_lcp, output);
}

char *serializer::unmarshal(std::istream &input, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp) {
    size_t len;
    input.read(reinterpret_cast<char *>(&len), sizeof(size_t));

    char *txt = new char[len + 1];
    txt[len] = '\0';
    ::string_view txt_sv = ::string_view::__unsafe_new(txt, len);
    WATCH(lz77::decompress(input, txt_sv));

    sa.resize(len);
    WATCH(input.read(reinterpret_cast<char *>(sa.data()), len * MULTIPLIER));

    l_lcp.resize(len);
    ::string_view l_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<char *>(l_lcp.data()), len * MULTIPLIER);
    WATCH(lz77::decompress(input, l_lcp_sv));

    r_lcp.resize(len);
    ::string_view r_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<char *>(r_lcp.data()), len * MULTIPLIER);
    WATCH(lz77::decompress(input, r_lcp_sv));

    return txt;
}

char *serializer::unmarshal(const std::string &s, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp) {
    std::fstream input(s, std::ios::in | std::ios::binary);
    return serializer::unmarshal(input, sa, l_lcp, r_lcp);
}
