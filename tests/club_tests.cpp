#include <gtest/gtest.h>
#include <sstream>
#include "club.h"

static const MyTime OPENING_TIME{.mm = 8 * 60};
static const MyTime CLOSING_TIME{.mm = 23 * 60};
static const int HOUR_PRICE = 10;

TEST(ClubTest, ClientNormalArriveAndLeave) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });
    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(clientName, 1)
    });

    ASSERT_EQ(2, events.size());
    ASSERT_EQ(EventID::CLIENT_ARRIVE, events[0].id);
    ASSERT_EQ(EventID::CLIENT_SIT, events[1].id);
}

TEST(ClubTest, ClientArriveWhenNotOpen) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });
    club.processEvent(Event{
        .tm = {7 * 60 + 59},
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });

    ASSERT_EQ(2, events.size());
    ASSERT_EQ(EventID::ERROR, events[1].id);
    ASSERT_EQ(Error::NOT_OPEN_YET, std::get<Error>(events[1].payload));
}

TEST(ClubTest, PlaceIsBusy) {
    auto client1Name = Name{"client1"};
    auto client2Name = Name{"client2"};
    std::vector<Event> events;

    auto club = Club(5, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });

    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = client1Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(client1Name, 1)
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 2},
        .id = EventID::CLIENT_ARRIVE,
        .payload = client2Name
    });
        club.processEvent(Event{
        .tm = {8 * 60 + 3},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(client2Name, 1)
    });


    ASSERT_EQ(5, events.size());
    ASSERT_EQ(EventID::ERROR, events[4].id);
    ASSERT_EQ(Error::PLACE_IS_BUSY, std::get<Error>(events[4].payload));
}

TEST(ClubTest, ClientUnknown) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });
    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(clientName, 1)
    });

    ASSERT_EQ(2, events.size());
    ASSERT_EQ(EventID::ERROR, events[1].id);
    ASSERT_EQ(Error::CLIENT_UNKNOWN, std::get<Error>(events[1].payload));
}


TEST(ClubTest, YouShallNotPass) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });
    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });
    club.processEvent(Event{
        .tm = {.mm = 8 * 60 + 1},
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });

    ASSERT_EQ(3, events.size());
    ASSERT_EQ(EventID::ERROR, events[2].id);
    ASSERT_EQ(Error::YOU_SHALL_NOT_PASS, std::get<Error>(events[2].payload));
}


TEST(ClubTest, ICanWaitNoLonger) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });
    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });
    club.processEvent(Event{
        .tm = {.mm = 8 * 60 + 1},
        .id = EventID::CLIENT_WAIT,
        .payload = clientName
    });

    ASSERT_EQ(3, events.size());
    ASSERT_EQ(EventID::ERROR, events[2].id);
    ASSERT_EQ(Error::I_CAN_WAIT_NO_LONGER, std::get<Error>(events[2].payload));
}

TEST(ClubTest, AutoSitFromQueue) {
    auto client1Name = Name{"client1"};
    auto client2Name = Name{"client2"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });

    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = client1Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(client1Name, 1)
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 2},
        .id = EventID::CLIENT_ARRIVE,
        .payload = client2Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 3},
        .id = EventID::CLIENT_WAIT,
        .payload = client2Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 4},
        .id = EventID::CLIENT_LEAVE,
        .payload = client1Name
    });

    ASSERT_EQ(6, events.size());
    ASSERT_EQ(EventID::CLIENT_AUTO_SIT, events[5].id);
    auto [name, place] = std::get<NameTable>(events[5].payload);
    ASSERT_EQ(client2Name.name, name.name);
    ASSERT_EQ(1, place);
}

TEST(ClubTest, AutoLeaveFromQueue) {
    auto client1Name = Name{"client1"};
    auto client2Name = Name{"client2"};
    auto client3Name = Name{"client3"};
    auto client4Name = Name{"client4"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });

    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = client1Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(client1Name, 1)
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 2},
        .id = EventID::CLIENT_ARRIVE,
        .payload = client2Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 3},
        .id = EventID::CLIENT_WAIT,
        .payload = client2Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 4},
        .id = EventID::CLIENT_ARRIVE,
        .payload = client3Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 5},
        .id = EventID::CLIENT_WAIT,
        .payload = client3Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 6},
        .id = EventID::CLIENT_ARRIVE,
        .payload = client4Name
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 7},
        .id = EventID::CLIENT_WAIT,
        .payload = client4Name
    });

    ASSERT_EQ(9, events.size());
    ASSERT_EQ(EventID::CLIENT_AUTO_LEAVE, events[8].id);
    ASSERT_EQ(client4Name.name, std::get<Name>(events[8].payload).name);
}

TEST(ClubTest, AutoLeaveAtEnd) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });

    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(clientName, 1)
    });
    club.processEndOfTheDay();

    ASSERT_EQ(3, events.size());
    ASSERT_EQ(EventID::CLIENT_AUTO_LEAVE, events[2].id);
    ASSERT_EQ(CLOSING_TIME.mm, events[2].tm.mm);
}

TEST(ClubTest, GetStats) {
    auto clientName = Name{"client1"};
    std::vector<Event> events;

    auto club = Club(1, OPENING_TIME, CLOSING_TIME, HOUR_PRICE, [&](const auto &event) {
        events.push_back(event);
    });

    club.processEvent(Event{
        .tm = OPENING_TIME,
        .id = EventID::CLIENT_ARRIVE,
        .payload = clientName
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 1},
        .id = EventID::CLIENT_SIT,
        .payload = std::make_pair(clientName, 1)
    });
    club.processEvent(Event{
        .tm = {8 * 60 + 62},
        .id = EventID::CLIENT_LEAVE,
        .payload = clientName
    });

    auto stats = club.getTablesStats();
    ASSERT_EQ(1, stats.size());
    ASSERT_EQ(2 * HOUR_PRICE, stats[0].first);
    ASSERT_EQ(61, stats[0].second.mm);
}