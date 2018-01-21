#!/bin/bash
# Script for building Shape on Linux machine

project=shape

#expected build dir structure
buildexp=build/Eclipse_CDT4-Unix_Makefiles

currentdir=$PWD
builddir=./${buildexp}

mkdir -p ${builddir}

#launch cmake to generate build environment
pushd ${builddir}
cmake -G "Eclipse CDT4 - Unix Makefiles" ${currentdir} -DCMAKE_BUILD_TYPE=Debug
popd

#build from generated build environment
cmake --build ${builddir}
