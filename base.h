#pragma once

#include <iostream>
#include <string>

struct MyTime {
    int mm;
};

struct Name {
    std::string name;

    bool operator < (const Name &other) const {
        return name < other.name;
    }
};


std::istream& operator >> (std::istream &in, MyTime &name);
std::ostream& operator << (std::ostream &out, const MyTime &name);

std::istream& operator >> (std::istream &in, Name &name);
std::ostream& operator << (std::ostream &out, const Name &name);