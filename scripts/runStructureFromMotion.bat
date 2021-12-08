@echo off

if [%1]==[] (
	echo Missing argument dfm or aliceVision
	exit
)

set target=%1

set PATH=%PATH%;%AliceVision_INSTALL%/bin
set PATH=%PATH%;%VCPKG_ROOT%/installed/x64-windows/bin

mkdir Cache\%target%\StructureFromMotion

set SFM_DATA=Cache/CameraInit/cameraInit.sfm
set SFM_ABC=Cache/%target%/StructureFromMotion/sfm.abc
set CAMERAS_SFM=Cache/%target%/StructureFromMotion/cameras.sfm
set EXTRA_INFO=Cache/%target%/StructureFromMotion

if "%target%"=="dfm" goto setDFMVariables
if "%target%"=="aliceVision" goto setAliceVisionVariables
:setDFMVariables
	set FEATURES=Cache/%target%/ExtractionAndMatching/regions
	set MATCHES=Cache/%target%/ExtractionAndMatching/matches
	goto setVariablesExit
:setAliceVisionVariables
	set FEATURES=Cache/%target%/FeatureExtraction/
	set MATCHES=Cache/%target%/FeatureMatching/
:setVariablesExit	
	
aliceVision_incrementalSfM.exe ^
	--input "%SFM_DATA%" ^
	--featuresFolders "%FEATURES%" ^
	--matchesFolders "%MATCHES%" ^
	--describerTypes sift ^
	--localizerEstimator acransac ^
	--observationConstraint Basic ^
	--localizerEstimatorMaxIterations 4096 ^
	--localizerEstimatorError 0.0 ^
	--lockScenePreviouslyReconstructed False ^
	--useLocalBA True ^
	--localBAGraphDistance 1 ^
	--maxNumberOfMatches 0 ^
	--minNumberOfMatches 0 ^
	--minInputTrackLength 2 ^
	--minNumberOfObservationsForTriangulation 2 ^
	--minAngleForTriangulation 3.0 ^
	--minAngleForLandmark 2.0 ^
	--maxReprojectionError 4.0 ^
	--minAngleInitialPair 5.0 ^
	--maxAngleInitialPair 40.0 ^
	--useOnlyMatchesFromInputFolder False ^
	--useRigConstraint True ^
	--lockAllIntrinsics False ^
	--filterTrackForks False ^
	--initialPairA "" ^
	--initialPairB "" ^
	--interFileExtension .abc ^
	--verboseLevel error ^
	--output "%SFM_ABC%" ^
	--outputViewsAndPoses "%CAMERAS_SFM%" ^
	--extraInfoFolder "%EXTRA_INFO%"
call :checkReturnCode "aliceVision_incrementalSfM"

exit /B 0

:checkReturnCode
if %ERRORLEVEL% NEQ 0 (
	if %ERRORLEVEL% == --1073741515 (
		echo %1 is missing required dependencies
	) else (
		echo %1 failed with error code %ERRORLEVEL%
	)
	exit
)
exit /B 0