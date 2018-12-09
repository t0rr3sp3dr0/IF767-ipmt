#ifndef IPMT_LZ78_H
#define IPMT_LZ78_H


#include <iostream>

#include "def.h"
#include "string_view.h"

namespace lz78 {
    void compress(const ::string_view &txt, std::ostream &out);
    void decompress(std::istream &in, ::string_view &txt);
}


#endif //IPMT_LZ78_H
