#include <aliceVision/sfmData/SfMData.hpp>
#include <aliceVision/sfmDataIO/sfmDataIO.hpp>
#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace avf = aliceVision::feature;

int main()
{
	std::string input = "imageMatches.txt";
	std::string output = "relations.txt";
	std::string sfmDataPath = "Cache/CameraInit/cameraInit.sfm";

	aliceVision::sfmData::SfMData sfmData;
	std::cout << "Reading sfmData from " << sfmDataPath << std::endl;
	auto esfmData = aliceVision::sfmDataIO::ESfMData(aliceVision::sfmDataIO::VIEWS);
	if (!aliceVision::sfmDataIO::Load(sfmData, sfmDataPath, esfmData)) {
		std::cout << "Failed to load sfmData from '" << sfmDataPath << "'!" << std::endl;
		return EXIT_FAILURE;
	}

	std::map<std::string, std::string> images;
	for (const auto& entry : sfmData.getViews()) {
		std::filesystem::path imagePath(entry.second.get()->getImagePath());
		std::string imageId = std::to_string(entry.second.get()->getViewId());
		images[imageId] = imagePath.stem().string();
		//std::cout << imageId << " <-> " << imagePath.stem().string() << std::endl;
	}

	std::ifstream inputStream;
	inputStream.open(input);

	std::ofstream outputStream;
	outputStream.open(output);

	std::string line;
	while (std::getline(inputStream, line)) {
		std::istringstream iss(line);
		std::string word;
		std::string source = "";
		while (std::getline(iss, word, ' ')) {
			if (source == "") {
				source = images[word];
			}
			else {
				outputStream << source << " " << images[word] << std::endl;
			}
		}
	}

	return 0;
}
