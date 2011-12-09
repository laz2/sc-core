mkdir build
cd build
cmake -G "Visual Studio 10" -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_TOOLS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
call "c:\Program Files\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"
devenv "sc-core.sln" /build "Debug" /project "INSTALL"
devenv "sc-core.sln" /build "RelWithDebInfo" /project "INSTALL"