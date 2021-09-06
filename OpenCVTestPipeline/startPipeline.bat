@echo off

setlocal EnableDelayedExpansion

set CONFIG=Release
set N_IMAGES=49

set PATH=%PATH%;%AliceVision_INSTALL%/%CONFIG%/bin
if "%CONFIG%"=="Release" (
	set PATH=!PATH!;%VCPKG_ROOT%/installed/x64-windows/bin
) else if "%CONFIG%"=="Debug" (
	set PATH=!PATH!;%VCPKG_ROOT%/installed/x64-windows/debug/bin
)

mkdir Cache\CameraInit
mkdir Cache\DepthMap
mkdir Cache\DepthMapFilter
mkdir Cache\ExtractionAndMatching
mkdir Cache\MeshFiltering
mkdir Cache\Meshing
mkdir Cache\PrepareDenseScene
mkdir Cache\StructureFromMotion
mkdir Cache\Texturing

set VIEWPOINTS=Cache/CameraInit/viewpoints.sfm
set SFM_DATA=Cache/CameraInit/cameraInit.sfm
set FEATURES=Cache/ExtractionAndMatching/regions
set MATCHES=Cache/ExtractionAndMatching/matches
set SFM_ABC=Cache/StructureFromMotion/sfm.abc
set CAMERAS_SFM=Cache/StructureFromMotion/cameras.sfm
set PREPARE_DENSE_SCENE=Cache/PrepareDenseScene
set DEPTH_MAP=Cache/DepthMap
set DEPTH_MAP_FILTER=Cache/DepthMapFilter
set MESH=Cache/Meshing/mesh.obj
set DENSE_POINT_CLOUD=Cache/Meshing/densePointCloud.abc
set FILTERED_MESH=Cache/MeshFiltering/mesh.obj
set TEXTURING=Cache/Texturing

aliceVision_cameraInit.exe ^
	--sensorDatabase "C:/Users/Administrator/Documents/Programms/Meshroom-2021.1.0/aliceVision/share/aliceVision/cameraSensors.db" ^
	--defaultFieldOfView 45.0 ^
	--groupCameraFallback folder ^
	--allowedCameraModels pinhole,radial1,radial3,brown,fisheye4,fisheye1 ^
	--useInternalWhiteBalance True ^
	--viewIdMethod metadata ^
	--verboseLevel info ^
	--output %SFM_DATA% ^
	--allowSingleView 1 ^
	--input %VIEWPOINTS%
call :checkReturnCode "aliceVision_cameraInit"

OpenCVTestPipeline.out\\build\\x64-%CONFIG%\\OpenCVTestPipeline\\OpenCVTestPipeline.exe ^
	--input Cache/CameraInit/cameraInit.sfm ^
	--output Cache/ExtractionAndMatching/ ^
	--threshold 6.0 ^
	--features 5000
call :checkReturnCode "OpenCVTestPipeline"

:incrementalSfM
aliceVision_incrementalSfM.exe ^
	--input %SFM_DATA% ^
	--featuresFolders %FEATURES% ^
	--matchesFolders %MATCHES% ^
	--describerTypes sift_ocv ^
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
	--verboseLevel info ^
	--output %SFM_ABC% ^
	--outputViewsAndPoses %CAMERAS_SFM% ^
	--extraInfoFolder "Cache/StructureFromMotion"
call :checkReturnCode "aliceVision_incrementalSfM"

:prepareDenseScene
aliceVision_prepareDenseScene ^
	--input %SFM_ABC% ^
	--outputFileType exr ^
	--saveMetadata True ^
	--saveMatricesTxtFiles False ^
	--evCorrection False ^
	--verboseLevel info ^
	--output %PREPARE_DENSE_SCENE% ^
	--rangeStart 0 ^
	--rangeSize N_IMAGES 
call :checkReturnCode "aliceVision_prepareDenseScene"

aliceVision_depthMapEstimation ^
	--input %SFM_ABC% ^
	--imagesFolder %PREPARE_DENSE_SCENE% ^
	--downscale 2 ^
	--minViewAngle 2.0 ^
	--maxViewAngle 70.0 ^
	--sgmMaxTCams 10 ^
	--sgmWSH 4 ^
	--sgmGammaC 5.5 ^
	--sgmGammaP 8.0 ^
	--refineMaxTCams 6 ^
	--refineNSamplesHalf 150 ^
	--refineNDepthsToRefine 31 ^
	--refineNiters 100 ^
	--refineWSH 3 ^
	--refineSigma 15 ^
	--refineGammaC 15.5 ^
	--refineGammaP 8.0 ^
	--refineUseTcOrRcPixSize False ^
	--exportIntermediateResults False ^
	--nbGPUs 0 ^
	--verboseLevel info ^
	--output %DEPTH_MAP% ^
	--rangeStart 0 ^
	--rangeSize N_IMAGES
call :checkReturnCode "aliceVision_depthMapEstimation"

aliceVision_depthMapFiltering ^
	--input %SFM_ABC% ^
	--depthMapsFolder %DEPTH_MAP% ^
	--minViewAngle 2.0 ^
	--maxViewAngle 70.0 ^
	--nNearestCams 10 ^
	--minNumOfConsistentCams 3 ^
	--minNumOfConsistentCamsWithLowSimilarity 4 ^
	--pixSizeBall 0 ^
	--pixSizeBallWithLowSimilarity 0 ^
	--computeNormalMaps False ^
	--verboseLevel info ^
	--output %DEPTH_MAP_FILTER% ^
	--rangeStart 0 ^
	--rangeSize N_IMAGES
call :checkReturnCode "aliceVision_depthMapFiltering"

aliceVision_meshing ^
	--input %SFM_ABC% ^
	--depthMapsFolder %DEPTH_MAP_FILTER% ^
	--estimateSpaceFromSfM True ^
	--estimateSpaceMinObservations 3 ^
	--estimateSpaceMinObservationAngle 10 ^
	--maxInputPoints 50000000 ^
	--maxPoints 5000000 ^
	--maxPointsPerVoxel 1000000 ^
	--minStep 2 ^
	--partitioning singleBlock ^
	--repartition multiResolution ^
	--angleFactor 15.0 ^
	--simFactor 15.0 ^
	--pixSizeMarginInitCoef 2.0 ^
	--pixSizeMarginFinalCoef 4.0 ^
	--voteMarginFactor 4.0 ^
	--contributeMarginFactor 2.0 ^
	--simGaussianSizeInit 10.0 ^
	--simGaussianSize 10.0 ^
	--minAngleThreshold 1.0 ^
	--refineFuse True ^
	--helperPointsGridSize 10 ^
	--nPixelSizeBehind 4.0 ^
	--fullWeight 1.0 ^
	--voteFilteringForWeaklySupportedSurfaces True ^
	--addLandmarksToTheDensePointCloud False ^
	--invertTetrahedronBasedOnNeighborsNbIterations 10 ^
	--minSolidAngleRatio 0.2 ^
	--nbSolidAngleFilteringIterations 2 ^
	--colorizeOutput False ^
	--maxNbConnectedHelperPoints 50 ^
	--saveRawDensePointCloud False ^
	--exportDebugTetrahedralization False ^
	--seed 0 ^
	--verboseLevel info ^
	--outputMesh %MESH% ^
	--output %DENSE_POINT_CLOUD% 
call :checkReturnCode "aliceVision_meshing"
	
aliceVision_meshFiltering ^
	--inputMesh %MESH% ^
	--keepLargestMeshOnly False ^
	--smoothingSubset all ^
	--smoothingBoundariesNeighbours 0 ^
	--smoothingIterations 5 ^
	--smoothingLambda 1.0 ^
	--filteringSubset all ^
	--filteringIterations 1 ^
	--filterLargeTrianglesFactor 60.0 ^
	--filterTrianglesRatio 0.0 ^
	--verboseLevel info ^
	--outputMesh %FILTERED_MESH% 
call :checkReturnCode "aliceVision_meshFiltering"

aliceVision_texturing ^
	--input %DENSE_POINT_CLOUD% ^
	--imagesFolder %PREPARE_DENSE_SCENE% ^
	--inputMesh %FILTERED_MESH% ^
	--textureSide 8192 ^
	--downscale 2 ^
	--outputTextureFileType png ^
	--unwrapMethod Basic ^
	--useUDIM True ^
	--fillHoles False ^
	--padding 5 ^
	--multiBandDownscale 4 ^
	--multiBandNbContrib 1 5 10 0 ^
	--useScore True ^
	--bestScoreThreshold 0.1 ^
	--angleHardThreshold 90.0 ^
	--processColorspace sRGB ^
	--correctEV False ^
	--forceVisibleByAllVertices False ^
	--flipNormals False ^
	--visibilityRemappingMethod PullPush ^
	--subdivisionTargetRatio 0.8 ^
	--verboseLevel info ^
	--output %TEXTURING% 
call :checkReturnCode "aliceVision_texturing"
exit

:checkReturnCode
if %ERRORLEVEL% NEQ 0 (
	echo %1 failed with error code %ERRORLEVEL%
	exit
)
exit /B 0