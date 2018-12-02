//
// Created by Pedro Tôrres on 2018-12-01.
//

#ifndef IPMT_LZ77_H
#define IPMT_LZ77_H


#include <fstream>
#include <utility>
#include <vector>

#include "def.h"
#include "string_view.h"

class lz77 {
private:
    static constexpr size_t ALPHABET_LENGTH = UCHAR_MAX + 1;

    template<typename T>
    static inline constexpr size_t hash(size_t i, const T &c) {
        return (i * lz77::ALPHABET_LENGTH) + static_cast<unsigned char>(c);
    }

    static void init_fsm(std::vector<size_t> &fsm, const ::string_view &pat);

    static std::pair<size_t, size_t> prefix_match(const ::string_view &win, const ::string_view &pat);

    template <size_t SIZE>
    static inline void integer_encode(const size_t &i, std::ostream &out) {
        out.write(reinterpret_cast<const char *>(&i), SIZE);
    }

    template <size_t SIZE>
    static inline size_t integer_decode(std::istream &in) {
        size_t i = 0;
        in.read(reinterpret_cast<char *>(&i), SIZE);
        return i;
    }

public:
    template <size_t WIN_BYTES = 2, size_t LAB_BYTES = 1>
    static inline void compress(const ::string_view &txt, std::ostream &out) {
        static_assert(WIN_BYTES > LAB_BYTES, "WIN_BYTES must be greater than LAB_BYTES");
        static_assert(LAB_BYTES > 0, "LAB_BYTES must be greater than 0");

        constexpr size_t win_len = static_cast<size_t>(1) << (WIN_BYTES << 3);
        constexpr size_t lab_len = static_cast<size_t>(1) << (LAB_BYTES << 3);

        const size_t n = txt.length();

        const size_t safe = std::min(win_len, n);
        std::string _before;
        _before.resize(win_len + safe);
        ::string_view before = _before;
        std::fill(_before.begin(), _before.begin() + win_len, '\0');
        std::copy(txt.begin(), txt.begin() + safe, _before.begin() + win_len);

        for (size_t i = 0; i < n;) {
            ::string_view win = i < win_len ? before(i + lab_len, i + lab_len + win_len) : txt(i + lab_len - win_len, std::min(i + lab_len, n));
            ::string_view pat = txt(i, std::min(i + lab_len, n));

            std::pair<size_t, size_t> p = lz77::prefix_match(win, pat);

            const size_t &pos = p.first;
            lz77::integer_encode<WIN_BYTES>(pos, out);

            const size_t &len = p.second;
            lz77::integer_encode<LAB_BYTES>(len, out);

            const char &c = pat[len];
            out.write(&c, sizeof(char));

            i += len + 1;
        }
    }

    template <size_t WIN_BYTES = 2, size_t LAB_BYTES = 1>
    static inline ::string_view decompress(std::istream &in, ::string_view &txt) {
        static_assert(WIN_BYTES > LAB_BYTES, "WIN_BYTES must be greater than LAB_BYTES");
        static_assert(LAB_BYTES > 0, "LAB_BYTES must be greater than 0");

        constexpr size_t win_len = static_cast<size_t>(1) << (WIN_BYTES << 3);
        constexpr size_t lab_len = static_cast<size_t>(1) << (LAB_BYTES << 3);

        char *_txt = const_cast<char *>(txt.begin());
        const size_t n = txt.length();

        for (size_t i = 0; i < n;) {
            size_t pos = lz77::integer_decode<WIN_BYTES>(in);
            size_t len = lz77::integer_decode<LAB_BYTES>(in);
            const char c = static_cast<const char>(in.get());

            if (len > 0 && pos + i + lab_len < win_len) {
                size_t diff = std::min(win_len - pos, len);
                std::fill(_txt + i, _txt + i + diff, '\0');

                len -= diff;
                pos += diff;
                i += diff;
            }

            if (len > 0)
                pos += i + lab_len - win_len;
            while (i < pos + len) {
                std::copy(_txt + pos, _txt + i, _txt + i);

                size_t diff = i - pos;
                len -= diff;
                pos += diff;
                i += diff;
            }
            std::copy(_txt + pos, _txt + pos + len, _txt + i);
            _txt[i + len] = c;

            i += len + 1;
        }

        return txt;
    }
};

inline void lz77::init_fsm(std::vector<size_t> &fsm, const ::string_view &pat) {
    const size_t m = pat.length();

    fsm.resize((m + 1) * lz77::ALPHABET_LENGTH);
    fsm[lz77::hash(0, pat[0])] = 1;

    size_t brb = 0;
    for (size_t i = 1; i < m; ++i) {
        for (size_t c = 0; c < lz77::ALPHABET_LENGTH; ++c)
            fsm[lz77::hash(i, c)] = fsm[hash(brb, c)];
        fsm[lz77::hash(i, pat[i])] = i + 1;
        brb = fsm[lz77::hash(brb, pat[i])];
    }

    for (size_t c = 0; c < lz77::ALPHABET_LENGTH; ++c)
        fsm[lz77::hash(m, c)] = fsm[hash(brb, c)];
}

inline std::pair<size_t, size_t> lz77::prefix_match(const ::string_view &win, const ::string_view &pat) {
    const size_t n = win.length();
    const size_t m = pat.length();
    const size_t diff = n - m;

    std::vector<size_t> fsm;
    lz77::init_fsm(fsm, pat);

    size_t len = 0;
    size_t pos = 0;
    size_t cur = 0;
    size_t idx = 0;
    for (auto &c : win) {
        cur = fsm[hash(cur, c)];
        if (cur > len && 1 + idx - cur < diff) {
            len = cur;
            pos = idx - cur + 1;
        }

        ++idx;
    }
    len = std::min(len, m - 1);

    return {pos, len};
}


#endif //IPMT_LZ77_H
