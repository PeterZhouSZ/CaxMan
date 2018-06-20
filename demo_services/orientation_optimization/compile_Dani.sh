#!/bin/sh

CINOLIB_INCLUDE_DIR=/home/daniela/Devel/lib/cinolib/include
EIGEN_INCLUDE_DIR=/media/daniela/Shared/Devel/lib/eigen3

mkdir -p build

g++ -std=c++11 -I$CINOLIB_INCLUDE_DIR -I$EIGEN_INCLUDE_DIR orientation_service.cpp -o./build/orient_service
