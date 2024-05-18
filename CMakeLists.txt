if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(yadro_entrance VERSION 1.0)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(club STATIC event.cpp base.cpp club.cpp)

add_executable(main main.cpp)
target_link_libraries(main club)


# FOR TESTS:
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(test_club tests/test.cpp tests/base_tests.cpp tests/event_tests.cpp tests/club_tests.cpp)
target_include_directories(test_club PRIVATE .)
target_link_libraries(test_club club GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_club)