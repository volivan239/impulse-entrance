#include "event.h"

std::istream& operator >> (std::istream &in, Event &event) {
    if (!(in >> event.tm)) {
        return in;
    }

    int id;
    if (!(in >> id)) {
        return in;
    }

    Name name;
    if (!(in >> name)) {
        return in;
    }

    int tableNum;

    if (id == (int) EventID::CLIENT_ARRIVE || id == (int) EventID::CLIENT_WAIT || id == (int) EventID::CLIENT_LEAVE) {
        event.payload = name;
    } else if (id == (int) EventID::CLIENT_SIT) {
        if (!(in >> tableNum)) {
            return in;
        }
        event.payload = std::make_pair(name, tableNum);
    } else {
        in.setstate(std::ios::failbit);
        return in;
    }

    event.id = (EventID) id;
    return in;
}

std::ostream& operator << (std::ostream &out, const Event &event) {
    out << event.tm << ' ' << (int) event.id << ' ';
    if (std::holds_alternative<Name>(event.payload)) {
        out << std::get<Name>(event.payload).name;
    } else if (std::holds_alternative<NameTable>(event.payload)) {
        auto [name, tableNum] = std::get<NameTable>(event.payload);
        out << name << ' ' << tableNum;
    } else {
        auto error = std::get<Error>(event.payload);
        if (error == Error::YOU_SHALL_NOT_PASS) {
            out << "YouShallNotPass";
        } else if (error == Error::NOT_OPEN_YET) {
            out << "NotOpenYet";
        } else if (error == Error::PLACE_IS_BUSY) {
            out << "PlaceIsBusy";
        } else if (error == Error::I_CAN_WAIT_NO_LONGER) {
            out << "ICanWaitNoLonger!";
        } else {
            out << "ClientUnknown";
        }
    }
    return out;
}