@echo off

mkdir matchVisualisation
mkdir Cache\dfm\ExtractionAndMatching
del /S /Q Cache\dfm\ExtractionAndMatching
del /S /Q matchVisualisation

bin\\DFMToAliceVision.exe ^
	--output Cache/dfm/ExtractionAndMatching/ ^
	--dfmPath ImageGraph/ ^
	--sfmFile Cache/CameraInit/cameraInit.sfm ^
	--printMatches true ^
	--useOutlierFiltering false ^
	--outlierThreshold 0.80 ^
	--useKeyPointFiltering true ^
	--keyPointThreshold 10 ^
	--maxFileLength 200000

if %ERRORLEVEL% NEQ 0 (
	echo DFMToAliceVision failed with error code %ERRORLEVEL%!
)
