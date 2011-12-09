mkdir build
cd build
cmake -G "Eclipse CDT4 - MinGW Makefiles" -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DSC_CORE_PYTHON_BINDINGS=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
mingw23-make install
