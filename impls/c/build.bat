@echo off

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4211 -wd4200

mkdir build 2>NUL
pushd build

REM 64-bit build
cl %CommonCompilerFlags% -FC -Z7 ..\step0_repl.cpp
popd
