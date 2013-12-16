cmake_minimum_required(VERSION 2.8)
project(xtpath)

add_subdirectory("external/pugi")

include_directories("external/boost/include")
include_directories("external")
link_directories("${xtpath_SOURCE_DIR}/external/boost/lib")
add_definitions(-std=c++11)
add_executable(testpugi test/test_xpath.cpp)

target_link_libraries (testpugi pugixml boost_unit_test_framework)
