#!/bin/sh

CINOLIB_INCLUDE_DIR=/Users/cino/Documents/research/devel/lib/CinoLib/include
EIGEN_INCLUDE_DIR=/Users/cino/Documents/research/devel/lib/CinoLib/external/eigen

mkdir -p build

g++ -std=c++11 -I$CINOLIB_INCLUDE_DIR -I$EIGEN_INCLUDE_DIR orientation_service.cpp -o./build/orient_service
