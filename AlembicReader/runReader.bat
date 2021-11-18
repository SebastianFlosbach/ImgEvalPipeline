@echo off

set root="C:\\Users\\Administrator\\Documents\\Projects\\OpenCVTestPipeline\\Cache\\StructureFromMotion\\sfm.abc"
set rootAV="C:\\Users\\Administrator\\Documents\\Data\\MeshroomCache\\StructureFromMotion\\fd32411d02050286a7503e1f07544a8706401032\\sfm.abc"

out\\build\\x64-Debug\\AlembicReader\\AlembicReader.exe ^
	--input %rootAV% ^
	--output "C:\\Users\\Administrator\\Documents\\Projects\\Evaluation\\poses_st_peters_square_av.txt"
call :checkReturnCode "AlembicReader"

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