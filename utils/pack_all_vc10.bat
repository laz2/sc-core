mkdir build
cd build

cmake -G "Visual Studio 10" -DPACK=1 -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_TOOLS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
if errorlevel 1 goto end

call "%VS100COMNTOOLS%\vsvars32.bat"
msbuild sc-core.sln /p:Configuration=Debug
if errorlevel 1 goto end

msbuild sc-core.sln /p:Configuration=Release
if errorlevel 1 goto end

msbuild INSTALL.vcxproj /p:Configuration=Debug
if errorlevel 1 goto end

msbuild INSTALL.vcxproj /p:Configuration=Release
if errorlevel 1 goto end

msbuild PACKAGE.vcxproj /p:Configuration=Release

:end
cd ..
