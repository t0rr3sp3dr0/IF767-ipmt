//
// Created by Pedro Tôrres on 2018-11-30.
//

#include "suffix_array.h"

#include <algorithm>
#include <cmath>

#include "serializer.h"
#include "watch.h"

template <typename T>
inline void suffix_array::init_ranking(std::vector<size_t> &v, const std::vector<T> &ts) {
    const size_t n = ts.size();

    std::vector<std::pair<T, size_t>> sorted(n);
    size_t i = 0;
    for (auto &t : ts) {
        sorted[i] = {t, i};
        ++i;
    }
    std::sort(sorted.begin(), sorted.end());

    size_t ranking = 0;
    T *prev = nullptr;
    for (auto &e : sorted) {
        if (prev && e.first != *prev)
            ++ranking;

        prev = &e.first;
        v[e.second] = ranking;
    }
}

inline void suffix_array::init_inverse_sa(std::vector<size_t> &inverse_sa, const ::string_view &s) {
    const size_t n = s.length();

    const auto l = static_cast<size_t>(ceil(log2(n)));

    std::vector<std::vector<size_t>> indexes;
    indexes.assign(2, std::vector<size_t>(n));

    std::vector<char> vc = std::vector<char>(s.begin(), s.end());
    suffix_array::init_ranking<char>(indexes[0], vc);

    bool b = true;
    for (size_t i = 1; i < l + 1; ++i, b = !b) {
        size_t j = static_cast<size_t>(1) << (i - 1);

        std::vector<std::pair<size_t, size_t>> ranking(n);
        for (size_t k = 0; k < n; ++k)
            if (j + k >= n)
                ranking[k] = {indexes[!b][k] + 1, 0};
            else
                ranking[k] = {indexes[!b][k] + 1, indexes[!b][j + k] + 1};

        suffix_array::init_ranking<std::pair<size_t, size_t>>(indexes[b], ranking);
    }

    inverse_sa = indexes[!b];
}

inline void suffix_array::invert_index(std::vector<size_t> &ret, const std::vector<size_t> &v) {
    const size_t n = v.size();

    ret.resize(n);
    for (size_t i = 0; i < n; ++i)
        ret[v[i]] = i;
}

inline void suffix_array::init_h_lcp(std::vector<size_t> &h_lcp, const ::string_view &s, const std::vector<size_t> &sa, const std::vector<size_t> &inverse_sa) {
    const size_t n = s.length();

    h_lcp.resize(n - 1);

    for (size_t i = 0, j = 0; i < n; i++, j -= j > 0) {
        size_t k = inverse_sa[i];
        if (k == n - 1) {
            j = 0;
            continue;
        }

        size_t l = sa[k + 1];
        while (i + j < n && l + j < n && s[i + j] == s[l + j])
            j++;

        h_lcp[k] = j;
    }
}

inline void suffix_array::_init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &h_lcp, const size_t &l, const size_t &r) {
    if (r - l <= 1)
        return;

    size_t h = (l + r) / 2;

    l_lcp[h] = static_cast<size_t>(-1);
    for (size_t i = l; i < h; ++i)
        l_lcp[h] = std::min(h_lcp[i], l_lcp[h]);

    r_lcp[h] = static_cast<size_t>(-1);
    for (size_t i = h; i < r; ++i)
        r_lcp[h] = std::min(h_lcp[i], r_lcp[h]);

    suffix_array::_init_lr_lcp(l_lcp, r_lcp, h_lcp, l, h);
    suffix_array::_init_lr_lcp(l_lcp, r_lcp, h_lcp, h, r);
}

inline void suffix_array::init_lr_lcp(std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp, const std::vector<size_t> &h_lcp, size_t n) {
    l_lcp.resize(n);
    r_lcp.resize(n);

    size_t l = 0;
    size_t r = n - 1;

    suffix_array::_init_lr_lcp(l_lcp, r_lcp, h_lcp, l, r);
}

inline size_t suffix_array::lcp_bf(const ::string_view &txt, const ::string_view &pat) {
    size_t i = 0;
    const char *const txt_end = txt.end();
    const char *const pat_end = pat.end();
    for (const char *txt_it = txt.begin(), *pat_it = pat.begin(); txt_it != txt_end && pat_it != pat_end && *txt_it == *pat_it; ++txt_it, ++pat_it, ++i);
    return i;
}

inline size_t suffix_array::succ(const ::string_view &txt, const ::string_view &pat, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp) {
    const size_t n = txt.length();
    const size_t m = pat.length();

    ::string_view ls = txt(sa[0]);
    ::string_view rs = txt(sa[n - 1]);

    size_t ll = std::min(ls.length(), pat.length());
    if (ls(0, ll) > pat(0, ll))
        return 0;

    size_t rl = std::min(rs.length(), pat.length());
    if (rs(0, rl) < pat(0, rl))
        return n;

    size_t ls_lcp = suffix_array::lcp_bf(ls, pat);
    size_t rs_lcp = suffix_array::lcp_bf(rs, pat);
    size_t hs_lcp;

    size_t l = 0;
    size_t r = n - 1;
    while (r - l > 1) {
        size_t h = (l + r) / 2;

        if (ls_lcp >= rs_lcp) {
            if (l_lcp[h] > ls_lcp) {
                l = h;
                continue;
            }

            if (l_lcp[h] < ls_lcp) {
                r = h;
                rs_lcp = l_lcp[h];
                continue;
            }

            hs_lcp = ls_lcp;
            if (ls_lcp < m)
                hs_lcp += suffix_array::lcp_bf(txt(sa[h] + ls_lcp), pat(ls_lcp));
        } else {
            if (r_lcp[h] > rs_lcp) {
                r = h;
                continue;
            }

            if (r_lcp[h] < rs_lcp) {
                l = h;
                ls_lcp = r_lcp[h];
                continue;
            }

            hs_lcp = rs_lcp;
            if (rs_lcp < m)
                hs_lcp += suffix_array::lcp_bf(txt(sa[h] + rs_lcp), pat(rs_lcp));
        }

        if (hs_lcp < m && sa[h] + hs_lcp < n && txt[sa[h] + hs_lcp] < pat[hs_lcp]) {
            l = h;
            ls_lcp = hs_lcp;
        } else {
            r = h;
            rs_lcp = hs_lcp;
        }
    }

    if (ls_lcp > rs_lcp)
        return l;

    return r;
}

inline size_t suffix_array::pred(const ::string_view &txt, const ::string_view &pat, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp) {
    const size_t n = txt.length();
    const size_t m = pat.length();

    ::string_view ls = txt(sa[0]);
    ::string_view rs = txt(sa[n - 1]);

    size_t ll = std::min(ls.length(), pat.length());
    if (ls(0, ll) > pat(0, ll))
        return static_cast<size_t>(-1);

    size_t rl = std::min(rs.length(), pat.length());
    if (rs(0, rl) < pat(0, rl))
        return n - 1;

    size_t ls_lcp = suffix_array::lcp_bf(ls, pat);
    size_t rs_lcp = suffix_array::lcp_bf(rs, pat);
    size_t hs_lcp;

    size_t l = 0;
    size_t r = n - 1;
    while (r - l > 1) {
        size_t h = (l + r) / 2;

        if (ls_lcp >= rs_lcp) {
            if (l_lcp[h] > ls_lcp) {
                l = h;
                continue;
            }

            if (l_lcp[h] < ls_lcp) {
                r = h;
                rs_lcp = l_lcp[h];
                continue;
            }

            hs_lcp = ls_lcp;
            if (ls_lcp < m)
                hs_lcp += suffix_array::lcp_bf(txt(sa[h] + ls_lcp), pat(ls_lcp));
        } else {
            if (r_lcp[h] > rs_lcp) {
                r = h;
                continue;
            }

            if (r_lcp[h] < rs_lcp) {
                l = h;
                ls_lcp = r_lcp[h];
                continue;
            }

            hs_lcp = rs_lcp;
            if (rs_lcp < m)
                hs_lcp += suffix_array::lcp_bf(txt(sa[h] + rs_lcp), pat(rs_lcp));
        }

        if (hs_lcp < m && hs_lcp < n - sa[h] && txt[sa[h] + hs_lcp] > pat[hs_lcp]) {
            r = h;
            rs_lcp = hs_lcp;
        } else {
            l = h;
            ls_lcp = hs_lcp;
        }
    }

    if (rs_lcp > ls_lcp)
        return r;

    return l;
}

suffix_array::suffix_array(const string_view &txt) : free(false), txt(txt) {
    std::vector<size_t> inverse_sa;
    WATCH(suffix_array::init_inverse_sa(inverse_sa, this->txt));

    WATCH(suffix_array::invert_index(this->sa, inverse_sa));

    std::vector<size_t> h_lcp;
    WATCH(suffix_array::init_h_lcp(h_lcp, this->txt, this->sa, inverse_sa));

    WATCH(suffix_array::init_lr_lcp(this->l_lcp, this->r_lcp, h_lcp, this->txt.length()));
}

suffix_array::suffix_array(std::istream &in) : free(true) {
    this->txt = serializer::unmarshal(in, this->sa, this->l_lcp, this->r_lcp);
}

suffix_array::~suffix_array() {
    if (this->free)
        delete[] this->txt.begin();
}

void suffix_array::find(std::vector<size_t> &occ, const ::string_view pat) const {
    size_t l = suffix_array::succ(this->txt, pat, this->sa, this->l_lcp, this->r_lcp);
    size_t r = suffix_array::pred(this->txt, pat, this->sa, this->l_lcp, this->r_lcp);

    if (l + 1 > r + 1)
        return;

    occ.resize(r - l + 1);
    for (size_t i = l; i <= r; ++i)
        occ[i - l] = this->sa[i];
}

size_t suffix_array::count(::string_view pat) const {
    size_t l = suffix_array::succ(this->txt, pat, this->sa, this->l_lcp, this->r_lcp);
    size_t r = suffix_array::pred(this->txt, pat, this->sa, this->l_lcp, this->r_lcp);

    if (l + 1 > r + 1)
        return 0;

    return r - l + 1;
}

void suffix_array::marshal(std::ostream &out) const {
    serializer::marshal(this->txt, this->sa, this->l_lcp, this->r_lcp, out);
}
