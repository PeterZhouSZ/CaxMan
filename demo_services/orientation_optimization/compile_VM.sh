#!/bin/sh

CINOLIB_INCLUDE_DIR=~/third_party_libs/cinolib/include
EIGEN_INCLUDE_DIR=~/third_party_libs/eigen

mkdir -p build

g++ -std=c++11 -I$CINOLIB_INCLUDE_DIR -I$EIGEN_INCLUDE_DIR orientation_service.cpp -o./build/orient_service
