mkdir build
cd build
cmake -G "Visual Studio 9 2008" -DPACK=1 -DSC_CORE_BUILD_TOOLS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
if errorlevel 1 goto end

call "c:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat"
msbuild sc-core.sln /p:Configuration=Debug
if errorlevel 1 goto end

msbuild sc-core.sln /p:Configuration=RelWithDebInfo
if errorlevel 1 goto end

msbuild INSTALL.vcproj /p:Configuration=Debug
if errorlevel 1 goto end

msbuild INSTALL.vcproj /p:Configuration=RelWithDebInfo
if errorlevel 1 goto end

msbuild PACKAGE.vcproj /p:Configuration=RelWithDebInfo

:end
cd ..
