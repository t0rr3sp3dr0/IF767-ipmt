//
// Created by Pedro Tôrres on 2018-11-30.
//

#include "serializer.h"

#include <fstream>
#include <vector>

#include "lz77.hpp"
#include "watch.h"

static constexpr size_t MULTIPLIER = sizeof(size_t) / sizeof(char);

std::function<void(const ::string_view &, std::ostream &)> serializer::compressor = [](const ::string_view &sv, std::ostream &os) {  // NOLINT(cert-err58-cpp)
    os << sv;
};

std::function<void(std::istream &, ::string_view &)> serializer::decompressor = [](std::istream &is, ::string_view &sv) {  // NOLINT(cert-err58-cpp)
    for (auto &e : sv) {
        char *b = const_cast<char *>(&e);
        *b = static_cast<char>(is.get());
    }
};

void serializer::marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, std::ostream &output) {
    const size_t len = txt.length();
    output.write(reinterpret_cast<const char *>(&len), sizeof(size_t));

    WATCH(serializer::compressor(txt, output));

    WATCH(output.write(reinterpret_cast<const char *>(sa.data()), len * MULTIPLIER));

    ::string_view l_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<const char *>(l_lcp.data()), len * MULTIPLIER);
    WATCH(serializer::compressor(l_lcp_sv, output));

    ::string_view r_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<const char *>(r_lcp.data()), len * MULTIPLIER);
    WATCH(serializer::compressor(r_lcp_sv, output));
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
    WATCH(serializer::decompressor(input, txt_sv));

    sa.resize(len);
    WATCH(input.read(reinterpret_cast<char *>(sa.data()), len * MULTIPLIER));

    l_lcp.resize(len);
    ::string_view l_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<char *>(l_lcp.data()), len * MULTIPLIER);
    WATCH(serializer::decompressor(input, l_lcp_sv));

    r_lcp.resize(len);
    ::string_view r_lcp_sv = ::string_view::__unsafe_new(reinterpret_cast<char *>(r_lcp.data()), len * MULTIPLIER);
    WATCH(serializer::decompressor(input, r_lcp_sv));

    return txt;
}

char *serializer::unmarshal(const std::string &s, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp) {
    std::fstream input(s, std::ios::in | std::ios::binary);
    return serializer::unmarshal(input, sa, l_lcp, r_lcp);
}
