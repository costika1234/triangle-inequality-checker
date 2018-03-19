#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Clone googletest from GitHub.
cd "${DIR}/../lib" && git clone https://github.com/google/googletest.git

GTEST_DIR=googletest/googletest

# Find appropriate compiler.
UNAME=`uname -s`
if [[ ${UNAME} == "Linux" ]]; then
    CXX=g++
elif [[ ${UNAME} == "Darwin" ]]; then
    CXX=clang++
fi

# Build and archive gtest.
${CXX} -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
ar -rv libgtest.a gtest-all.o

# Move headers to current directory.
mkdir -p gtestheaders
mv ${GTEST_DIR}/include/gtest gtestheaders

# Clean-up.
rm -rf googletest
rm -rf *.o