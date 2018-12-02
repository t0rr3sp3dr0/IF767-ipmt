//
// Created by Pedro Tôrres on 2018-12-02.
//

#ifndef IPMT_DC3_H
#define IPMT_DC3_H


#include <climits>
#include <vector>

#include "def.h"
#include "string_view.h"

namespace dc3 {
    static constexpr size_t ALPHABET_LENGTH = UCHAR_MAX + 1;

    void suffix_array(std::vector<size_t> &sa, const size_t *txt, size_t len, size_t alphabet = dc3::ALPHABET_LENGTH);

    void suffix_array(std::vector<size_t> &sa, const ::string_view &txt, size_t alphabet = dc3::ALPHABET_LENGTH);
};


#endif //IPMT_DC3_H
