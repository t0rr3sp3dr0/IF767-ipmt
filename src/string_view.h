//
// Created by Pedro Tôrres on 2018-11-30.
//

#ifndef IPMT_STRING_VIEW_H
#define IPMT_STRING_VIEW_H


#include <cstring>
#include <ostream>
#include <string>

#include "def.h"

class string_view {
private:
    const char *data;
    size_t size;

    string_view(const char *data, size_t size);

public:
    string_view();

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    string_view(const char *s);
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    string_view(const std::string &s);
#pragma clang diagnostic pop

    static string_view __unsafe_new(const char *data, size_t size);

    string_view &operator=(const char *s);

    string_view &operator=(const std::string &s);

    bool operator==(const string_view &rhs) const;

    bool operator!=(const string_view &rhs) const;

    bool operator<(const string_view &rhs) const;

    bool operator>(const string_view &rhs) const;

    bool operator<=(const string_view &rhs) const;

    bool operator>=(const string_view &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const string_view &sv);

    const char &operator[](size_t index) const;

    string_view operator()(size_t begin, size_t end = static_cast<size_t>(-1)) const;

    const size_t &length() const;

    const char *begin() const;

    const char *end() const;
};


#endif //IPMT_STRING_VIEW_H
