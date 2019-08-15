#!/bin/bash

DG_VER=1.8.14

if [ $# -gt 0 ]; then
    DG_VER=$1
fi

if [ ! -n "$CPU_NUM" ]; then
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then 
        export CPU_NUM=`grep -c ^processor /proc/cpuinfo`
    elif [ "$TRAVIS_OS_NAME" == "osx" ]; then
        export CPU_NUM=`sysctl -n hw.logicalcpu_max`
    fi
fi

wget -q https://sourceforge.net/projects/doxygen/files/rel-$DG_VER/doxygen-$DG_VER.src.tar.gz
gunzip doxygen-$DG_VER.src.tar.gz && tar xf doxygen-$DG_VER.src.tar 
cd doxygen-$DG_VER
mkdir build && cd build
cmake -G "Unix Makefiles" .. && make -s -j$CPU_NUM && sudo make install
cd ../
rm -rf doxygen-$DG_VER
