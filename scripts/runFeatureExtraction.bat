@echo off

set PATH=%PATH%;%AliceVision_INSTALL%/bin
set PATH=%PATH%;%VCPKG_ROOT%/installed/x64-windows/bin

mkdir Cache\aliceVision\FeatureExtraction
del /S /Q Cache\aliceVision\FeatureExtraction

set nbImages=0
for %%A in (visualize/*.jpg) do set /a nbImages+=1

aliceVision_featureExtraction.exe ^
	--input "Cache/CameraInit/cameraInit.sfm" ^
	--describerTypes sift ^
	--describerPreset normal ^
	--describerQuality normal ^
	--contrastFiltering GridSort ^
	--gridFiltering True ^
	--forceCpuExtraction True ^
	--maxThreads 0 ^
	--verboseLevel error ^
	--output "Cache/aliceVision/FeatureExtraction" ^
	--rangeStart 0 ^
	--rangeSize %nbImages%
	
if %ERRORLEVEL% NEQ 0 (
	echo aliceVision_featureExtraction failed with error code %ERRORLEVEL%!
	exit
)
