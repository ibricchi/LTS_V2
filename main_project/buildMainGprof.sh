#!/bin/bash

cmake -DCMAKE_CXX_FLAGS=-pg -Bbuild -H.
cd build
make
cp LTS_V2 ../LTS_V2_GPROF
cd ../