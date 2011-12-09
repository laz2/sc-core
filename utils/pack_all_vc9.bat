mkdir build
cd build

cmake -G "Visual Studio 9 2008" -DPACK=1 -DSC_CORE_BUILD_DOCS=1 -DSC_CORE_BUILD_TOOLS=1 -DSC_CORE_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..
if errorlevel 1 goto end

if exist "%VS90COMNTOOLS%..\..\vc\bin\nmake.exe" (
	echo installed msvc-2008
) else (
	echo no MSVC installed!
	exit /b 1
)

call "%VS90COMNTOOLS%\vsvars32.bat"
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
