@echo off

del /F /Q "_CPack_Packages"
rmdir /Q /S "_CPack_Packages"

del /F /Q PACKAGE.dir
rmdir /Q /S PACKAGE.dir

del /F /Q build
rmdir /Q /S build

del /F /Q INSTALL.dir
rmdir /Q /S INSTALL.dir

del /F /Q ALL_BUILD.dir
rmdir /Q /S ALL_BUILD.dir

del /F /Q ZERO_CHECK.dir
rmdir /Q /S ZERO_CHECK.dir

del /F /Q CMakeFiles
rmdir /Q /S CMakeFiles

del /F /Q ipch
rmdir /Q /S ipch

del /F /Q Debug
rmdir /Q /S Debug

del /F /Q RelWithDebInfo
rmdir /Q /S RelWithDebInfo

del /F /Q  _ReSharper.sc-core
rmdir /Q /S _ReSharper.sc-core

del /F /Q CMakeCache.txt
del /F /Q *.vcproj
del /F /Q *.vcxproj
del /F /Q *.filters
del /F /Q *.sdf
del /F /Q *.cmake
del /F /Q *.sln
del /F /Q *.user
del /F /Q install_manifest.txt
del /F /Q *.ncb
del /F /Q *.suo
del sc-core.suo /A:H /F /Q
del /F /Q Makefile

del /F /Q sc-core.resharper

del .cproject
del .project

del /F /Q *.sln.cache
del /F /Q *.exe

