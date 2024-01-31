@echo off

cd ..
call Vendor\PreMake\premake5.exe vs2022

cd Scripts
call python compile_shaders.py
PAUSE