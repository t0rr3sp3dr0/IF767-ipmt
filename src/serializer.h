//
// Created by Pedro Tôrres on 2018-11-30.
//

#ifndef IPMT_SERIALIZER_H
#define IPMT_SERIALIZER_H


#include <iostream>
#include <vector>

#include "def.h"
#include "string_view.h"

namespace serializer {
    void marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, std::ostream &output);
    void marshal(const ::string_view &txt, const std::vector<size_t> &sa, const std::vector<size_t> &l_lcp, const std::vector<size_t> &r_lcp, const std::string &s);
    char *unmarshal(std::istream &input, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp);
    char *unmarshal(const std::string &s, std::vector<size_t> &sa, std::vector<size_t> &l_lcp, std::vector<size_t> &r_lcp);
};


#endif //IPMT_SERIALIZER_H
