mkdir build
cd build
cmake -G "Visual Studio 7 .NET 2003" -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
call "c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
devenv "sc-core.sln" /build "Debug" /project "INSTALL"
devenv "sc-core.sln" /build "RelWithDebInfo" /project "INSTALL"