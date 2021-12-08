@echo off

if [%1]==[] (
	echo Missing argument dfm or aliceVision
	exit
)

set target=%1

set nbImages=0
for %%A in (visualize/*.jpg) do set /a nbImages+=1

set PATH=%PATH%;%AliceVision_INSTALL%/bin
set PATH=%PATH%;%VCPKG_ROOT%/installed/x64-windows/bin

mkdir Cache\%target%\DepthMap
mkdir Cache\%target%\DepthMapFilter
mkdir Cache\%target%\MeshFiltering
mkdir Cache\%target%\Meshing
mkdir Cache\%target%\PrepareDenseScene
mkdir Cache\%target%\Texturing

set SFM_DATA=Cache/CameraInit/cameraInit.sfm
set FEATURES=Cache/%target%/ExtractionAndMatching/regions
set MATCHES=Cache/%target%/ExtractionAndMatching/matches
set SFM_ABC=Cache/%target%/StructureFromMotion/sfm.abc
set CAMERAS_SFM=Cache/%target%/StructureFromMotion/cameras.sfm
set PREPARE_DENSE_SCENE=Cache/%target%/PrepareDenseScene
set DEPTH_MAP=Cache/%target%/DepthMap
set DEPTH_MAP_FILTER=Cache/%target%/DepthMapFilter
set MESH=Cache/%target%/Meshing/mesh.obj
set DENSE_POINT_CLOUD=Cache/%target%/Meshing/densePointCloud.abc
set FILTERED_MESH=Cache/%target%/MeshFiltering/mesh.obj
set TEXTURING=Cache/%target%/Texturing

aliceVision_prepareDenseScene.exe ^
	--input %SFM_ABC% ^
	--outputFileType exr ^
	--saveMetadata True ^
	--saveMatricesTxtFiles False ^
	--evCorrection False ^
	--verboseLevel info ^
	--output %PREPARE_DENSE_SCENE% ^
	--rangeStart 0 ^
	--rangeSize %nbImages% 
call :checkReturnCode "aliceVision_prepareDenseScene"

aliceVision_depthMapEstimation.exe ^
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
	--rangeSize %nbImages% 
call :checkReturnCode "aliceVision_depthMapEstimation"

aliceVision_depthMapFiltering.exe ^
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
	--rangeSize %nbImages% 
call :checkReturnCode "aliceVision_depthMapFiltering"

aliceVision_meshing.exe ^
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

aliceVision_meshFiltering.exe ^
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

aliceVision_texturing.exe ^
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
	if %ERRORLEVEL% == --1073741515 (
		echo %1 is missing required dependencies
	) else (
		echo %1 failed with error code %ERRORLEVEL%
	)
	exit
)
exit /B 0