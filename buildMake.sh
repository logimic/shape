#!/bin/bash
# Script for building shape on Linux machine

project=shape

#expected build dir structure
buildexp=build/Unix_Makefiles

currentdir=$PWD
builddir=./${buildexp}

mkdir -p ${builddir}

#launch cmake to generate build environment
pushd ${builddir}
cmake -G "Unix Makefiles" -DBUILD_TESTING:BOOL=true -DCMAKE_BUILD_TYPE=Debug ${currentdir}
popd

#build from generated build environment
cmake --build ${builddir}
