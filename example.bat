@echo off


title Your batch process name



REM change active dir to current location
%~d0
cd /d "%~dp0"



if "%~1" neq "oneinstance" (
	SingleInstanceCmd.exe "%~n0" "%~0" "oneinstance"
	goto :eof
)



REM if not defined iammaximized (
REM     set iammaximized=1
REM     start "" /max /wait "%~0"
REM     exit
REM )



REM change screen dimensions
REM mode con: cols=200 lines=9999



:loop


C:\path\to\your\program.exe your program arguments > log.txt 2>&1


REM ping -n 2 127.0.0.1
sleep 1


goto loop
