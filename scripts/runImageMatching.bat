@echo off

set nbMatches=0

if not [%1] == [] set nbMatches=%1

set PATH=%PATH%;%AliceVision_INSTALL%/bin

mkdir Cache\ImageMatching
del /Q /S Cache\ImageMatching
del /Q relations.txt

rem VocabularyTree Exhaustive
aliceVision_imageMatching ^
	--input Cache/CameraInit/cameraInit.sfm ^
	--featuresFolders "Cache/aliceVision/FeatureExtraction" ^
	--method VocabularyTree ^
	--verboseLevel error ^
	--minNbImages 0 ^
	--nbMatches %nbMatches% ^
	--tree "bin/vlfeat_K80L3.SIFT.tree" ^
	--output "Cache/ImageMatching/imageMatches.txt"
	
bin\\GenerateImageMatches.exe
