set PATH=%PATH%;%AliceVision_INSTALL%/Release/bin

aliceVision_imageMatching ^
	--input "C:/Users/Administrator/Documents/Data/GL3D/MeshroomCache/CameraInit/7069d6652333a248ae475e5503d5add4e8c88f08/cameraInit.sfm" ^
	--featuresFolders "C:/Users/Administrator/Documents/Data/GL3D/MeshroomCache/FeatureExtraction/b0bf3100568f78079d87e005541328be04571adb" ^
	--method VocabularyTree ^
	--verboseLevel info ^
	--minNbImages 0 ^
	--nbMatches 8 ^
	--tree "C:/Users/Administrator/Documents/Programms/Meshroom-2021.1.0/aliceVision/share/aliceVision/vlfeat_K80L3.SIFT.tree" ^
	--output "C:/Users/Administrator/Documents/Projects/OpenCVTestPipeline/imageMatches.txt"