@echo off

set PATH=%PATH%;%AliceVision_INSTALL%/bin
set PATH=%PATH%;%VCPKG_ROOT%/installed/x64-windows/bin

mkdir Cache\aliceVision\FeatureMatching
del /S /Q Cache\aliceVision\FeatureMatching

set nbImages=0
for %%A in (visualize/*.jpg) do set /a nbImages+=1

rem ANN_L2, CASCADE_HASHING_L2
aliceVision_featureMatching ^
	--input "Cache/CameraInit/cameraInit.sfm" ^
	--featuresFolders "Cache/aliceVision/FeatureExtraction/" ^
	--imagePairsList "Cache/ImageMatching/imageMatches.txt" ^
	--describerTypes sift ^
	--photometricMatchingMethod ANN_L2 ^
	--geometricEstimator acransac ^
	--geometricFilterType fundamental_matrix ^
	--distanceRatio 0.8 ^
	--maxIteration 2048 ^
	--geometricError 2.1 ^
	--knownPosesGeometricErrorMax 5.0 ^
	--maxMatches 0 ^
	--savePutativeMatches False ^
	--crossMatching False ^
	--guidedMatching False ^
	--matchFromKnownCameraPoses False ^
	--exportDebugFiles False ^
	--verboseLevel error ^
	--output "Cache/aliceVision/FeatureMatching/" ^
	--rangeStart 0 ^
	--rangeSize %nbImages%

if %ERRORLEVEL% NEQ 0 (
	echo aliceVision_featureMatching failed with error code %ERRORLEVEL%!
	exit
)
