//
// Created by Pedro Tôrres on 2018-12-02.
//

#ifndef IPMT_DC3_H
#define IPMT_DC3_H


#include <climits>
#include <iostream>
#include <vector>

#include "def.h"
#include "string_view.h"

class dc3 {
private:
    const bool free;
    ::string_view txt;
    std::vector<size_t> sa;
    std::vector<size_t> l_lcp;
    std::vector<size_t> r_lcp;

    static constexpr size_t ALPHABET_LENGTH = UCHAR_MAX + 1;

    static void init_suffix_array(std::vector<size_t> &sa, const size_t *txt, size_t len,
                                  size_t alphabet = dc3::ALPHABET_LENGTH);

    static void init_suffix_array(std::vector<size_t> &sa, const ::string_view &txt,
                                  size_t alphabet = dc3::ALPHABET_LENGTH);

    static void invert_index(std::vector<size_t> &ret, const std::vector<size_t> &v);

    static void init_h_lcp(std::vector<size_t> &h_lcp, const ::string_view &s, const std::vector<size_t> &sa, const std::vector<size_t> &inverse_sa);

    static void _init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &h_lcp, const size_t &l, const size_t &r);

    static void init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &h_lcp, size_t n);

public:
    explicit dc3(const string_view &txt);

    explicit dc3(std::istream &in);

    virtual ~dc3();

    void marshal(std::ostream &out) const;
};


#endif //IPMT_DC3_H
