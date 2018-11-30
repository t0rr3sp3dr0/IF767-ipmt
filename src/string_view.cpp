//
// Created by Pedro Tôrres on 2018-11-30.
//

#include "string_view.h"

string_view::string_view(const char *data, size_t size) : data(data), size(size) {}

string_view::string_view() : data(nullptr), size(0) {}

string_view::string_view(const char *s) : data(s), size(strlen(s)) {}

string_view::string_view(const std::string &s) : data(s.c_str()), size(s.length()) {}

string_view &string_view::operator=(const char *s) {
    this->data = s;
    this->size = strlen(s);
    return *this;
}

string_view &string_view::operator=(const std::string &s) {
    this->data = s.c_str();
    this->size = s.length();
    return *this;
}

bool string_view::operator==(const string_view &rhs) const {
    if (this->size != rhs.size)
        return false;

    for (size_t i = 0; i < this->size; ++i)
        if (this->data[i] != rhs.data[i])
            return false;

    return true;
}

bool string_view::operator!=(const string_view &rhs) const {
    return !(rhs == *this);
}

bool string_view::operator<(const string_view &rhs) const {
    const size_t len = std::min(this->size, rhs.size);
    for (size_t i = 0; i < len; ++i)
        if (this->data[i] != rhs.data[i])
            return this->data[i] < rhs.data[i];

    return this->size < rhs.size;
}

bool string_view::operator>(const string_view &rhs) const {
    return rhs < *this;
}

bool string_view::operator<=(const string_view &rhs) const {
    return !(rhs < *this);
}

bool string_view::operator>=(const string_view &rhs) const {
    return !(*this < rhs);
}

std::ostream &operator<<(std::ostream &os, const string_view &view) {
    for (size_t i = 0; i < view.size; ++i)
        os << view.data[i];
    return os;
}

const char &string_view::operator[](size_t index) const {
    if (index >= this->size)
        throw std::out_of_range(std::to_string(index) + " >= " + std::to_string(this->size));

    return this->data[index];
}

string_view string_view::operator()(size_t begin, size_t end) const {
    if (begin >= this->size || begin > end)
        throw std::out_of_range(std::to_string(begin) + " >= " + std::to_string(this->size) + " || " + std::to_string(begin) + " > " + std::to_string(end));

    return string_view{this->data + begin, std::min(this->size, end) - begin};
}

const size_t &string_view::length() const {
    return this->size;
}

const char *string_view::begin() const {
    return this->data;
}

const char *string_view::end() const {
    return this->data + this->size;
}
