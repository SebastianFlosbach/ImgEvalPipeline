set PATH=%PATH%;%AliceVision_INSTALL%/Release/bin

aliceVision_imageMatching ^
	--input "c:/Users/Administrator/Documents/Data/MeshroomCache/StructureFromMotion/22e9e0804614e2d65ec512ce9f0ec30bf5093f5b/sfm.abc" ^
	--featuresFolders "c:/Users/Administrator/Documents/Data/MeshroomCache/FeatureExtraction/933795cf5b0476d007ddd817f6127a827df49240" ^
	--method VocabularyTree ^
	--verboseLevel info ^
	--minNbImages 0 ^
	--nbMatches 8 ^
	--tree "C:/Users/Administrator/Documents/Programms/Meshroom-2021.1.0/aliceVision/share/aliceVision/vlfeat_K80L3.SIFT.tree" ^
	--output "C:/Users/Administrator/Documents/Projects/OpenCVTestPipeline/imageMatches.txt"