@echo off
setlocal EnableDelayedExpansion

rem GoldHEN Plugins build script

cd /D "%~dp0"

echo [+] Workdir: !CD!

rd /s /q bin
mkdir bin\plugins
set BINDIR=!CD!\bin\plugins
echo [+] Output directory: !BINDIR!

rem change these if you wish:
set CC=clang
set CXX=clang++
set LD=ld.lld
set GH_SDK_SRC=%GOLDHEN_SDK%\source
set GH_SDK=%GOLDHEN_SDK%

rem usually you won't need to change anything below this line
set DEFS=-D_BSD_SOURCE=1 -D__BSD_VISIBLE=1 -D__PS4__=1 -DOO=1 -D__OPENORBIS__=1 -D__OOPS4__=1 -D__FINAL__=1
set LIBS=-lGoldHEN_Hook -lkernel -lc -lc++ -lSceVideoOut -lSceScreenShot -lSceVideoRecording -lSceSysmodule -lSceSystemService 
set COMMONFLAGS=--target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -isysroot "%OO_PS4_TOOLCHAIN%" -isystem "%OO_PS4_TOOLCHAIN%\include" -I"%GOLDHEN_SDK%\include" -Wno-c99-designator %DEFS%
set CXXFLAGS=-fexceptions -fcxx-exceptions -isystem "%OO_PS4_TOOLCHAIN%\include\c++\v1"
set LDFLAGS=-m elf_x86_64 -pie -e _init --script "%OO_PS4_TOOLCHAIN%\link.x" --eh-frame-hdr -L"%OO_PS4_TOOLCHAIN%\lib" -L%GH_SDK% %LIBS%

set datetimef=%DATE% %TIME%
for /f %%i in ('git rev-parse HEAD') do set COMMIT=%%i
for /f %%i in ('git branch --show-current') do set BRANCH=%%i
for /f %%i in ('git rev-list HEAD --count') do set VER=%%i

echo #pragma once /* git_ver.h */     >  common\git_ver.h
echo #define GIT_COMMIT "%COMMIT%"    >> common\git_ver.h
echo #define GIT_VER    "%BRANCH%"    >> common\git_ver.h
echo #define GIT_NUM    %VER%         >> common\git_ver.h
echo #define BUILD_DATE "%datetimef%" >> common\git_ver.h

echo [+] Building the plugins
for /D %%G in ("plugin-src\*") do (
    rd /s /q "%%G\build"
    mkdir "%%G\build"
    mkdir "%%G\build\ghsdk"
    set OBJS=
    rem C source, this ensures we build the CRT first and it's the first object file
    for %%f in ("common\*.c", "%%G\source\*.c") do (
        %CC% %COMMONFLAGS% -I"%%G\include" -c "%%f" -o "%%G\build\%%~nf.c.o"
        set OBJS=!OBJS! "%%G\build\%%~nf.c.o"
    )
    rem ugly but works, get the *name* of the directory
    for %%f in ("%%G") do set PLUGIN_NAME=%%~nxf

    echo [+] !PLUGIN_NAME!
    echo [+] !OBJS!
    %LD% %LDFLAGS% !OBJS! -o "%%G\build\!PLUGIN_NAME!.elf"
    "%OO_PS4_TOOLCHAIN%\bin\windows\create-fself" -in="%%G\build\!PLUGIN_NAME!.elf" -out="%%G\build\!PLUGIN_NAME!.oelf" -lib="%%G\build\!PLUGIN_NAME!.prx" --paid 0x3800000000000011
    copy "%%G\build\!PLUGIN_NAME!.prx" "!BINDIR!"
)

echo [+] All done
