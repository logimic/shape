#!/bin/bash
# Script for building shape on Linux machine

project=shape

#expected build dir structure
buildexp=build/Unix_Makefiles
deploydir=deploy/Unix_Makefiles

currentdir=$PWD
builddir=./${buildexp}

mkdir -p ${builddir}

#get path to deploy
mkdir -p ${deploydir}
pushd ${deploydir}
deploydir=$PWD
popd

#launch cmake to generate build environment
pushd ${builddir}
cmake -G "Unix Makefiles" -DBUILD_TESTING:BOOL=true -DCMAKE_BUILD_TYPE=Release -DSHAPE_DEPLOY:PATH=${deploydir} ${currentdir}
popd

#build from generated build environment
cmake --build ${builddir} --config Release --target install


