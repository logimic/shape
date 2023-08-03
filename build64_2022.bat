set project=shape

rem //expected build dir structure
set buildexp=build\\VS17_2022_x64

set currentdir=%cd%
set builddir=.\\%buildexp%
set deploydir=.\\deploy\\VS15_2022_x64

mkdir %builddir%

rem //get absolute path to to deploydir
mkdir %deploydir%
pushd %deploydir%
set deploydir=%cd%
popd

rem //launch cmake to generate build environment
pushd %builddir%
cmake -G "Visual Studio 17 2022" -DBUILD_TESTING:BOOL=true -DSHAPE_DEPLOY:PATH=%deploydir% %currentdir% 
popd

rem //build from generated build environment
cmake --build %builddir% --config Debug --target install
cmake --build %builddir% --config Release --target install
