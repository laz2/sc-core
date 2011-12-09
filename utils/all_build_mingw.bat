mkdir build
cd build
cmake -G "MinGW Makefiles" -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
devenv "sc-core.sln" /build "Debug" /project "INSTALL"
devenv "sc-core.sln" /build "RelWithDebInfo" /project "INSTALL"