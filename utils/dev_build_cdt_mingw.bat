@echo off
cmake -G "Eclipse CDT4 - MinGW Makefiles" -DSC_CORE_BUILD_EXAMPLES=1 -DSC_CORE_BUILD_TOOLS=1 -DCMAKE_INSTALL_PREFIX="./sc-core" .
