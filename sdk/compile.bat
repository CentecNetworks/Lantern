call c:\Tornado2.2\host\x86-win32\bin\torvars.bat

#simulation: vxworks-sim; board:vxworks;
set BOARD=vxworks-sim
set VER=d
set CPU=SIMNT
set ARCH=i686
set CC=ccsimpc
set AR=arsimpc
set AS=ccsimpc
set LD=ldsimpc
set RANLIB=ranlibsimpc

set MK_DIR= Z:/humber_sdk/mk
set SDK_DIR= Z:/humber_sdk
set CURDIR= Z:/humber_sdk
set WIND_BASE=C:/Tornado2.2
set TGT_DIR=C:/Tornado2.2/target
set PROJ_DIR=C:/Tornado2.2/target
set VX_HEADER=C:/Tornado2.2/target/h
set PRJ_TYPE=vxworks
set TOOL_FAMILY=gnu
set WIND_HOST_TYPE=x86-win32

@echo.
@echo      ==============================================
@echo      =       make Humber SDK                      =
@echo      ==============================================


del /q Y:/humber_sdk/ctccli/vxworks
make targetbase=vxworks clean -f Makefile
make targetbase=vxworks -f Makefile


