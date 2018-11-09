set project=shape

rem //expected build dir structure
set buildexp=build\\VS15_2017_x64

set currentdir=%cd%
set builddir=.\\%buildexp%
set libsdir=..\\..\\..

mkdir %builddir%

rem //launch cmake to generate build environment
pushd %builddir%
cmake -G "Visual Studio 15 2017 Win64" -DBUILD_TESTING:BOOL=true %currentdir% 
popd

rem //build from generated build environment
cmake --build %builddir%
