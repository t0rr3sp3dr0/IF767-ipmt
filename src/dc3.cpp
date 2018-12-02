//
// Created by Pedro Tôrres on 2018-12-02.
//

#include "dc3.h"

inline constexpr bool leq(size_t a1, size_t a2, size_t b1, size_t b2) {
    return a1 < b1 || (a1 == b1 && a2 <= b2);
}

inline constexpr bool leq(size_t a1, size_t a2, size_t a3, size_t b1, size_t b2, size_t b3) {
    return a1 < b1 || (a1 == b1 && leq(a2, a3, b2, b3));
}

inline void radix_sort(const std::vector<size_t> &in, std::vector<size_t> &sorted, const size_t *txt, size_t len, size_t alphabet) {
    std::vector<size_t> v(alphabet + 1);

    for (size_t i = 0; i < len; i++)
        v[txt[in[i]]]++;

    size_t sum = 0;
    for (auto &e : v) {
        size_t t = e;
        e = sum;
        sum += t;
    }

    for (size_t i = 0; i < len; i++)
        sorted[v[txt[in[i]]]++] = in[i];
}

void dc3::suffix_array(std::vector<size_t> &sa, const size_t *txt, size_t len, size_t alphabet) {
    sa.resize(len + 3);
    sa[len] = sa[len + 1] = sa[len + 2] = 0;

    const size_t n0 = (len + 2) / 3;
    const size_t n1 = (len + 1) / 3;
    const size_t n2 = len / 3;
    const size_t n02 = n0 + n2;

    std::vector<size_t> txt0(n0);
    std::vector<size_t> txt12(n02 + 3);
    txt12[n02] = txt12[n02 + 1] = txt12[n02 + 2] = 0;

    std::vector<size_t> sa0(n0);
    std::vector<size_t> sa12(n02 + 3);
    sa12[n02] = sa12[n02 + 1] = sa12[n02 + 2] = 0;

    for (size_t i = 0, j = 0; i < len + (n0 - n1); ++i)
        if (i % 3 != 0)
            txt12[j++] = i;

    radix_sort(txt12, sa12, txt + 2, n02, alphabet);
    radix_sort(sa12, txt12, txt + 1, n02, alphabet);
    radix_sort(txt12, sa12, txt, n02, alphabet);

    auto c0 = static_cast<size_t>(-1);
    auto c1 = static_cast<size_t>(-1);
    auto c2 = static_cast<size_t>(-1);
    size_t abc = 0;
    for (size_t i = 0; i < n02; i++) {
        if (txt[sa12[i]] != c0 || txt[sa12[i] + 1] != c1 || txt[sa12[i] + 2] != c2) {
            ++abc;
            c0 = txt[sa12[i]];
            c1 = txt[sa12[i] + 1];
            c2 = txt[sa12[i] + 2];
        }
        if (sa12[i] % 3 == 1)
            txt12[sa12[i] / 3] = abc;
        else
            txt12[sa12[i] / 3 + n0] = abc;
    }

    if (abc < n02) {
        suffix_array(sa12, txt12.data(), n02, abc);
        for (size_t i = 0; i < n02; ++i)
            txt12[sa12[i]] = i + 1;
    } else
        for (size_t i = 0; i < n02; ++i)
            sa12[txt12[i] - 1] = i;

    for (size_t i = 0, j = 0; i < n02; ++i)
        if (sa12[i] < n0)
            txt0[j++] = 3 * sa12[i];
    radix_sort(txt0, sa0, txt, n0, alphabet);

    for (size_t p = 0, t = n0 - n1, k = 0; k < len; ++k) {
        size_t i = sa12[t] < n0 ? sa12[t] * 3 + 1 : (sa12[t] - n0) * 3 + 2;
        size_t j = sa0[p];
        if (sa12[t] < n0 ? leq(txt[i], txt12[sa12[t] + n0], txt[j], txt12[j / 3]) : leq(txt[i], txt[i + 1], txt12[sa12[t] - n0 + 1], txt[j], txt[j + 1], txt12[j / 3 + n0])) {
            sa[k] = i;
            ++t;
            if (t == n02)
                for (++k; p < n0; ++p, ++k)
                    sa[k] = sa0[p];
        } else {
            sa[k] = j;
            ++p;
            if (p == n0)
                for (++k; t < n02; ++t, ++k)
                    sa[k] = sa12[t] < n0 ? sa12[t] * 3 + 1 : (sa12[t] - n0) * 3 + 2;
        }
    }
}

void dc3::suffix_array(std::vector<size_t> &sa, const ::string_view &txt, size_t alphabet) {
    const size_t n = txt.length();

    size_t a[txt.length() + 3];
    a[n] = a[n + 1] = a[n + 2] = 0;

    size_t i = 0;
    for (auto &c : txt)
        a[i++] = static_cast<size_t>(c);

    dc3::suffix_array(sa, a, n, alphabet);
}
