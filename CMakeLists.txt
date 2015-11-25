cmake_minimum_required(VERSION 2.8)
project(xtpath)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )

add_definitions(-std=c++11)
add_executable(testpugi test/test_xpath.cpp)

target_link_libraries (testpugi pugixml ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

