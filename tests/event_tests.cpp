#include <gtest/gtest.h>
#include <sstream>
#include "event.h"

TEST(EventTest, ArriveEventInput) {
    std::stringstream ss;
    ss << "12:34 1 client1";

    Event actual;
    ss >> actual;
    ASSERT_EQ(12 * 60 + 34, actual.tm.mm);
    ASSERT_EQ(EventID::CLIENT_ARRIVE, actual.id);
    ASSERT_EQ("client1", std::get<Name>(actual.payload).name);
}

TEST(EventTest, SitEventInput) {
    std::stringstream ss;
    ss << "01:01 2 client2 3";

    Event actual;
    ss >> actual;

    ASSERT_EQ(1 * 60 + 1, actual.tm.mm);
    ASSERT_EQ(EventID::CLIENT_SIT, actual.id);
    ASSERT_EQ("client2", std::get<NameTable>(actual.payload).first.name);
    ASSERT_EQ(3, std::get<NameTable>(actual.payload).second);
}

TEST(EventTest, WaitEventInput) {
    std::stringstream ss;
    ss << "12:39 3 client3";

    Event actual;
    ss >> actual;
    ASSERT_EQ(12 * 60 + 39, actual.tm.mm);
    ASSERT_EQ(EventID::CLIENT_WAIT, actual.id);
    ASSERT_EQ("client3", std::get<Name>(actual.payload).name);
}

TEST(EventTest, LeaveEventInput) {
    std::stringstream ss;
    ss << "23:00 4 client-128";

    Event actual;
    ss >> actual;
    ASSERT_EQ(23 * 60 + 00, actual.tm.mm);
    ASSERT_EQ(EventID::CLIENT_LEAVE, actual.id);
    ASSERT_EQ("client-128", std::get<Name>(actual.payload).name);
}

TEST(EventTest, InvalidIDEventInput) {
    std::stringstream ss;
    ss << "23:00 0 client-128";

    Event actual;
    ss >> actual;
    ASSERT_FALSE(ss);
}

TEST(EventTest, InvalidFormatEventInput) {
    std::stringstream ss;
    ss << "23:00 1";

    Event actual;
    ss >> actual;
    ASSERT_FALSE(ss);
}
