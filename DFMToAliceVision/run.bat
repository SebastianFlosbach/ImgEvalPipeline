@echo off

del processedImages.txt /q

:loop
DFMToAliceVision.out\\build\\x64-Release\\DFMToAliceVision\\DFMToAliceVision.exe
if %ERRORLEVEL% NEQ 0 goto loop
