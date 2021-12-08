@echo off

if [%1]==[] (
	exit
)

set target=%1

Evaluation\\bin\\AlembicReader.exe ^
	--input Cache/%target%/StructureFromMotion/sfm.abc ^
	--output Cache/%target%/StructureFromMotion/poses.txt
call :checkReturnCode "AlembicReader"

exit /B 0

:checkReturnCode
if %ERRORLEVEL% NEQ 0 (
	if %ERRORLEVEL% == -1073741515 (
		echo %1 is missing required dependencies
	) else (
		echo %1 failed with error code %ERRORLEVEL%
	)
	exit
)
exit /B 0