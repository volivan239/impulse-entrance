#include <iostream>
#include <sstream>
#include <fstream>
#include "base.h"
#include "event.h"
#include "club.h"

std::pair<Club, std::vector<Event>> readInput(std::ifstream &fin) {
    int nTables;
    MyTime openingTime, closingTime;
    int hourPrice;

    std::stringstream ss1, ss2, ss3;
    std::string line1, line2, line3;

    getline(fin, line1);
    ss1 << line1;
    if (!(ss1 >> nTables)) {
        std::cerr << "Parsing error at line 1: " << line1 << std::endl;
        exit(1);
    }

    getline(fin, line2);
    ss2 << line2;
    if (!(ss2 >> openingTime >> closingTime)) {
        std::cerr << "Parsing error at line 2: " << line2 << std::endl;
        exit(1);
    }

    getline(fin, line3);
    ss3 << line3;
    if (!(ss3 >> hourPrice)) {
        std::cerr << "Parsing error at line 3: " << line3 << std::endl;
        exit(1);
    }

    Club club(nTables, openingTime, closingTime, hourPrice, [](const Event &event) {
        std::cout << event << '\n';
    });

    std::string eventStr;
    int lineNumber = 3;
    std::vector<Event> events;
    while (getline(fin, eventStr)) {
        ++lineNumber;
        std::stringstream ss;
        ss << eventStr;
        
        Event curEvent;
        if (!(ss >> curEvent) || (!events.empty() && events.back().tm.mm >= curEvent.tm.mm)) {
            std::cerr << "Error at line " << lineNumber << ": " << eventStr << std::endl;
            exit(1);
        }
        events.push_back(curEvent);
    }

    return {club, events};
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        std::cerr << "Expected excatly one argument: input file name" << std::endl;
        return -1;
    }
    std::ifstream fin(argv[1]);

    auto [club, events] = readInput(fin);
    std::cout << club.getOpeningTime() << '\n';

    for (const auto &event : events) {
        club.processEvent(event);
    }
    club.processEndOfTheDay();

    std::cout << club.getClosingTime() << '\n';
    auto stats = club.getTablesStats();
    for (int i = 0; i < (int) stats.size(); i++) {
        std::cout << i + 1 << ' ' << stats[i].first << ' ' << stats[i].second << '\n';
    }
    return 0;
}