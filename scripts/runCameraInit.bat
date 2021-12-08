@echo off

set PATH=%PATH%;%AliceVision_INSTALL%/bin
set PATH=%PATH%;%VCPKG_ROOT%/installed/x64-windows/bin

mkdir Cache\CameraInit
del /S /Q Cache\CameraInit

aliceVision_cameraInit.exe ^
	--sensorDatabase "bin/cameraSensors.db" ^
	--defaultFieldOfView 45.0 ^
	--groupCameraFallback folder ^
	--allowedCameraModels pinhole,radial1,radial3,brown,fisheye4,fisheye1 ^
	--useInternalWhiteBalance True ^
	--viewIdMethod metadata ^
	--verboseLevel error ^
	--output "Cache/CameraInit/cameraInit.sfm" ^
	--allowSingleView 1 ^
	--imageFolder "visualize"

if %ERRORLEVEL% NEQ 0 (
	echo aliceVision_cameraInit failed with error code %ERRORLEVEL%!
	exit
)
