#ifndef OUTPUT_WRITER_HPP
#define OUTPUT_WRITER_HPP


#include <filesystem>
#include <memory>
#include <fstream>
#include <string>
#include <vector>

#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

namespace avf = aliceVision::feature;

class OutputWriter {
public:
	OutputWriter(const std::string& basePath) : basePath_(basePath)
	{
		if (!std::filesystem::exists(basePath_)) {
			std::filesystem::create_directories(basePath_);
		}
	}

	void clearOutputDirectory() {
		std::filesystem::remove_all(basePath_);
		std::filesystem::create_directories(basePath_ + regionsDirectory);
		std::filesystem::create_directories(basePath_ + matchesDirectory);

		currentMatchFileIndex_ = 0;
		currentMatchFileLength_ = 0;
	}

	void createOutputDirectory() {
		std::filesystem::create_directories(basePath_ + regionsDirectory);
		std::filesystem::create_directories(basePath_ + matchesDirectory);
	}

	void writeRegions(int imageId, std::unique_ptr<avf::SIFT_Regions>& regions) {
		std::string featureFile = basePath_ + regionsDirectory + std::to_string(imageId) + ".sift.feat";
		std::string descriptorFile = basePath_ + regionsDirectory + std::to_string(imageId) + ".sift.desc";

		regions->Save(featureFile, descriptorFile);
	}

	void writeMatches(const std::string& firstImageId, const std::string& secondImageId, const std::vector<cv::DMatch>& matches) {
		std::string outfilePath = basePath_ + matchesDirectory + std::to_string(currentMatchFileIndex_) + ".matches.txt";

		std::ofstream outfile;
		outfile.open(outfilePath, std::ios::app);

		outfile << firstImageId << " " << secondImageId << std::endl;
		outfile << "1" << std::endl;
		outfile << "sift " << matches.size() << std::endl;

		for (const auto& entry : matches) {
			outfile << entry.queryIdx << " " << entry.trainIdx << std::endl;
		}

		currentMatchFileLength_ += 3 + matches.size();
		if (currentMatchFileLength_ >= matchFileMaxLength_) {
			currentMatchFileIndex_++;
			currentMatchFileLength_ = 0;
		}
	}

private:
	std::string basePath_;
	int currentMatchFileIndex_ = 0;
	int currentMatchFileLength_ = 0;
	int matchFileMaxLength_ = 25000;

	const std::string regionsDirectory = "/regions/";
	const std::string matchesDirectory = "/matches/";

};


#endif
