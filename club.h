#pragma once

#include <functional>
#include <optional>
#include <vector>
#include <set>
#include <map>
#include "base.h"
#include "event.h"

typedef std::pair<int, MyTime> TableStats;

class Club {
public:
    Club(int nTables, MyTime openingTime, MyTime closingTime, int hourPrice, std::function<void(const Event&)> eventCallback);
    
    MyTime getOpeningTime() const;
    MyTime getClosingTime() const;

    void processEvent(const Event &event);
    void processEndOfTheDay();

    std::vector<TableStats> getTablesStats() const;

private:
    void processArrival(const MyTime &tm, const Name &name);
    void processSit(const MyTime &tm, const Name &name, int tableNum);
    void processWait(const MyTime &tm, const Name &name);
    void processLeave(const MyTime &tm, const Name &name);

    void leaveTable(const MyTime &tm, int tableNum);

private:

    struct cmp {
        bool operator() (std::pair<Name, MyTime> a, std::pair<Name, MyTime> b) const {
            return a.second.mm < b.second.mm;
        }
    };

    const int nTables_;
    const MyTime openingTime_, closingTime_;
    const int hourPrice_;
    const std::function<void(const Event&)> eventCallback_;

    std::vector<std::optional<std::pair<Name, MyTime>>> tables_;
    std::set<Name> inside_;
    std::map<Name, int> sitting_;
    std::map<Name, MyTime> waitingStartTime_;
    std::set<std::pair<Name, MyTime>, cmp> waiting_;

    std::vector<int> profit_;
    std::vector<MyTime> workTime_;
};
