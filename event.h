#pragma once

#include <string>
#include <iostream>
#include <variant>
#include "base.h"

typedef std::pair<Name, int> NameTable;

enum class EventID {
    CLIENT_ARRIVE = 1,
    CLIENT_SIT = 2,
    CLIENT_WAIT = 3,
    CLIENT_LEAVE = 4,

    CLIENT_AUTO_LEAVE = 11,
    CLIENT_AUTO_SIT = 12,
    ERROR = 13,
};

enum class Error {
    YOU_SHALL_NOT_PASS,
    NOT_OPEN_YET,
    PLACE_IS_BUSY,
    CLIENT_UNKNOWN,
    I_CAN_WAIT_NO_LONGER
};

struct Event {
    MyTime tm;
    EventID id;
    std::variant<Name, NameTable, Error> payload;
};

std::istream& operator >> (std::istream &in, Event &event);
std::ostream& operator << (std::ostream &out, const Event &event);