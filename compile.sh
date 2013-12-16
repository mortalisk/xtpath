#!/bin/sh
#g++ -c -o libpugi.o pugi/pugixml.cpp
#ar rcs libpugi.a libpugi.o

#g++ justpugi.cpp namespace_util.cpp -L. -lpugi -I. -std=c++11 -o justpugi -O3
#g++ xtpathpugi.cpp -L. -lpugi -I. -std=c++11 -o xtpathpugi -O3

g++ test/test_xpath.cpp -Lexternal/boost/lib -L. -lpugixml -lboost_unit_test_framework -Iexternal/boost/include/ -Iexternal -I. -std=c++11 -o testpugi

