@echo off

del /F /Q "build"
rmdir /Q /S "build"

del /F /Q "dist"
rmdir /Q /S "dist"

del /F /Q sc_core\pm.py
del /F /Q sc_core\tgf.py
del /F /Q *.cpp
del /F /Q *.h
