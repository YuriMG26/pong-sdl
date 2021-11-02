@echo off

set opts=-FC -GR- -EHa- -nologo -Zi
set code=%cd%
set sdl_path=/I C:\SDL\include /link /LIBPATH:C:\SDL\lib\x64
set sdl_libs=SDL2.lib SDL2main.lib SDL2_ttf.lib /SUBSYSTEM:CONSOLE
pushd ..\build\
cl %opts% %code%\main.c -Fegame.exe %sdl_path% %sdl_libs%
popd
