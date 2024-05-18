#include <gtest/gtest.h>
#include <sstream>
#include "base.h"

TEST(BaseTest, TimeInputPositive) {
    std::stringstream ss;
    ss << "07:29";

    MyTime expected{.mm = 7 * 60 + 29};
    MyTime actual;
    ss >> actual;

    ASSERT_EQ(expected.mm, actual.mm);
    ASSERT_TRUE(ss);
}

TEST(BaseTest, TimeInputNegative) {
    std::stringstream ss;
    ss << "27:29";

    MyTime tm;
    ss >> tm;
    ASSERT_FALSE(ss);
}

TEST(BaseTest, TimeOutput) {
    std::stringstream ss;
    MyTime tm{.mm = 12 * 60 + 7};
    ss << tm;

    std::string actual;
    ss >> actual;
    std::string expected = "12:07";
    ASSERT_EQ(expected, actual);
}

TEST(BaseTest, NamePositive) {
    std::stringstream ss;
    std::string goodName = "12_client-34";
    ss << goodName;

    Name actual;
    ss >> actual;

    ASSERT_EQ(goodName, actual.name);
    ASSERT_TRUE(ss);
}

TEST(BaseTest, NameNegative) {
    std::stringstream ss;
    std::string badName = "UpperCaseLettersAreNotAllowed";
    ss << badName;

    Name name;
    ss >> name;

    ASSERT_FALSE(ss);
}