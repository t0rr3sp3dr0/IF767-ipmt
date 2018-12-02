//
// Created by Pedro Tôrres on 2018-11-30.
//

#ifndef IPMT_SUFFIX_ARRAY_H
#define IPMT_SUFFIX_ARRAY_H


#include <istream>
#include <vector>

#include "def.h"
#include "string_view.h"

class suffix_array {
private:
    const bool free;
    ::string_view txt;
    std::vector<size_t> sa;
    std::vector<size_t> l_lcp;
    std::vector<size_t> r_lcp;

    template <typename T>
    static void init_ranking(std::vector<size_t> &v, const std::vector<T> &ts);

    static void init_indexes(std::vector<std::vector<size_t>> &indexes, const ::string_view &s);

    static void invert_index(std::vector<size_t> &ret, const std::vector<size_t> &v);

    static size_t lcp(const std::vector<std::vector<size_t>> &indexes, const size_t &n, size_t start, size_t end);

    static void _init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &sa, const std::vector<std::vector<size_t>> &indexes, const size_t &n, const size_t &l, const size_t &r);

    static void init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &sa, const std::vector<std::vector<size_t>> &indexes, size_t n);

    static size_t lcp_bf(const ::string_view &txt, const ::string_view &pat);

    static size_t succ(const ::string_view &txt, const ::string_view &pat, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp);

    static size_t pred(const ::string_view &txt, const ::string_view &pat, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp);

public:
    explicit suffix_array(const string_view &txt);

    explicit suffix_array(std::istream &in);

    virtual ~suffix_array();

    void find(std::vector<size_t> &occ, ::string_view pat) const;

    size_t count(::string_view pat) const;

    void marshal(std::ostream &out) const;
};


#endif //IPMT_SUFFIX_ARRAY_H
