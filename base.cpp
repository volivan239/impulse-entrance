#include "base.h"

std::istream& operator >> (std::istream &in, MyTime &tm) {
    std::string timeStr;
    in >> timeStr;

    if (timeStr.length() != 5 || timeStr[2] != ':' || !isdigit(timeStr[0]) 
        || !isdigit(timeStr[1]) || !isdigit(timeStr[3]) || !isdigit(timeStr[4])) {
        
        in.setstate(std::ios::failbit);
        return in;
    }

    int hh = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int mm = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
    if (hh > 23 || mm > 59) {
        in.setstate(std::ios::failbit);
        return in;
    }

    tm.mm = hh * 60 + mm;
    return in;
}

std::ostream& operator << (std::ostream &out, const MyTime &tm) {
    int hh = tm.mm / 60;
    int mm = tm.mm % 60;
    out << hh / 10 << hh % 10 << ':' << mm / 10 << mm % 10;
    return out;
}


std::istream& operator >> (std::istream &in, Name &name) {
    std::string nameStr;
    in >> nameStr;

    if (nameStr.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (const char &x : nameStr) {
        bool valid = (x >= 'a' && x <= 'z') || (x >= '0' && x <= '9') || (x == '_') || (x == '-');
        if (!valid) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    name.name = nameStr;
    return in;
}

std::ostream& operator << (std::ostream &out, const Name &name) {
    out << name.name;
    return out;
}