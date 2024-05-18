#include "club.h"

Club::Club(int nTables, MyTime openingTime, MyTime closingTime, int hourPrice, std::function<void(const Event&)> eventCallback):
    nTables_(nTables), openingTime_(openingTime), closingTime_(closingTime), hourPrice_(hourPrice), eventCallback_(eventCallback) {
    
    tables_.assign(nTables, std::nullopt);
    profit_.assign(nTables, 0);
    workTime_.assign(nTables, MyTime{.mm = 0});
}

MyTime Club::getOpeningTime() const {
    return openingTime_;
}

MyTime Club::getClosingTime() const {
    return closingTime_;
}

void Club::processEvent(const Event &event) {
    eventCallback_(event);
    if (event.id == EventID::CLIENT_ARRIVE) {
        processArrival(event.tm, std::get<Name>(event.payload));
    } else if (event.id == EventID::CLIENT_SIT) {
        auto [name, tableNum] = std::get<NameTable>(event.payload);
        processSit(event.tm, name, tableNum);
    } else if (event.id == EventID::CLIENT_WAIT) {
        processWait(event.tm, std::get<Name>(event.payload));
    } else if (event.id == EventID::CLIENT_LEAVE) {
        processLeave(event.tm, std::get<Name>(event.payload));
    }
}

void Club::processEndOfTheDay() {
    for (const auto &name : inside_) {
        eventCallback_(Event{closingTime_, EventID::CLIENT_AUTO_LEAVE, name});
    }
    waiting_.clear();
    waitingStartTime_.clear();
    for (int tableNum = 1; tableNum <= nTables_; tableNum++) {
        if (tables_[tableNum - 1].has_value()) {
            leaveTable(closingTime_, tableNum);
        }
    }
    inside_.clear();
}

std::vector<TableStats> Club::getTablesStats() const {
    std::vector<TableStats> stats;
    for (int tableNum = 1; tableNum <= nTables_; tableNum++) {
        stats.emplace_back(profit_[tableNum - 1], workTime_[tableNum - 1]);
    }
    return stats;
}


void Club::processArrival(const MyTime &tm, const Name &name) {
    if (inside_.find(name) != inside_.end()) {
        eventCallback_(Event{tm, EventID::ERROR, Error::YOU_SHALL_NOT_PASS});
        return;
    }
    if (tm.mm < openingTime_.mm || tm.mm >= closingTime_.mm) {
        eventCallback_(Event{tm, EventID::ERROR, Error::NOT_OPEN_YET});
        return;
    }
    inside_.insert(name);
}

void Club::processSit(const MyTime &tm, const Name &name, int tableNum) {
    if (tableNum < 0 || tableNum > nTables_ || tables_[tableNum - 1].has_value()) {
        eventCallback_(Event{tm, EventID::ERROR, Error::PLACE_IS_BUSY});
        return;
    }
    if (inside_.find(name) == inside_.end()) {
        eventCallback_(Event{tm, EventID::ERROR, Error::CLIENT_UNKNOWN});
        return;
    }

    if (waitingStartTime_.find(name) != waitingStartTime_.end()) {
        // Erase from queue if needed
        auto tm = waitingStartTime_[name];
        waiting_.erase({name, tm});
        waitingStartTime_.erase(name);
    }
    if (sitting_.find(name) != sitting_.end()) {
        leaveTable(tm, sitting_[name]);
    }

    sitting_[name] = tableNum;
    tables_[tableNum - 1] = std::make_pair(name, tm);
}

void Club::processWait(const MyTime &tm, const Name &name) {
    if ((int) sitting_.size() < nTables_) {
        eventCallback_(Event{tm, EventID::ERROR, Error::I_CAN_WAIT_NO_LONGER});
        return;
    }
    if ((int) waiting_.size() > nTables_) {
        eventCallback_(Event{tm, EventID::CLIENT_AUTO_LEAVE, name});
        return;
    }

    waitingStartTime_[name] = tm;
    waiting_.insert({name, tm});
}

void Club::processLeave(const MyTime &tm, const Name &name) {
    if (inside_.find(name) == inside_.end()) {
        eventCallback_(Event{tm, EventID::ERROR, Error::CLIENT_UNKNOWN});
        return;
    }

    if (waitingStartTime_.find(name) != waitingStartTime_.end()) {
        // Erase from queue if we are in queue
        auto tm = waitingStartTime_[name];
        waitingStartTime_.erase(name);
        waiting_.erase({name, tm});
    } else if (sitting_.find(name) != sitting_.end()) {
        // Leave table if we are at table
        leaveTable(tm, sitting_[name]);
    }

    inside_.erase(name);
}

void Club::leaveTable(const MyTime &tm, int tableNum) {
    auto [name, startTime] = tables_[tableNum - 1].value();
    tables_[tableNum - 1] = std::nullopt;
    sitting_.erase(name);
    int passedTime = tm.mm - startTime.mm;
    workTime_[tableNum - 1].mm += passedTime;
    profit_[tableNum - 1] += (passedTime + 59) / 60 * hourPrice_;

    if (!waiting_.empty()) {
        auto name = (*waiting_.begin()).first;
        waiting_.erase(waiting_.begin());
        waitingStartTime_.erase(name);

        eventCallback_(Event{tm, EventID::CLIENT_AUTO_SIT, std::make_pair(name, tableNum)});
        sitting_[name] = tableNum;
        tables_[tableNum - 1] = {name, tm};
    }
}