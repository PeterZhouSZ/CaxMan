#!/bin/sh

CINOLIB_INCLUDE_DIR=/Users/cino/Documents/research/devel/lib/CinoLib/include
EIGEN_INCLUDE_DIR=/Users/cino/Documents/research/devel/lib/CinoLib/external/eigen
BOOST_DIR=/usr/local/include

mkdir -p build

g++ -std=c++11 -DCINOLIB_USES_BOOST -I$CINOLIB_INCLUDE_DIR -I$EIGEN_INCLUDE_DIR -I$BOOST_DIR supports_service.cpp -o./build/supports_service
