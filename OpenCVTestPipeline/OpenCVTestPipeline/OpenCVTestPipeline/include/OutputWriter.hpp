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
	OutputWriter(const std::string& featureFolder, const std::string& matchFolder) : featureFolder_(featureFolder), matchFolder_(matchFolder)
	{
		if (!std::filesystem::exists(featureFolder_)) {
			std::filesystem::create_directories(featureFolder_);
		}
		if (!std::filesystem::exists(matchFolder_)) {
			std::filesystem::create_directories(matchFolder_);
		}
	}

	void clearOutputDirectory() {
		std::filesystem::remove_all(featureFolder_);
		std::filesystem::remove_all(matchFolder_);
		std::filesystem::create_directories(featureFolder_);
		std::filesystem::create_directories(matchFolder_);

		currentMatchFileIndex_ = 0;
		currentMatchFileLength_ = 0;
	}

	void writeRegions(int imageId, std::unique_ptr<avf::SIFT_Regions>& regions) {
		std::string featureFile = featureFolder_ + std::to_string(imageId) + ".sift_ocv.feat";
		std::string descriptorFile = featureFolder_ + std::to_string(imageId) + ".sift_ocv.desc";

		regions->Save(featureFile, descriptorFile);
	}

	void writeMatches(const std::string& firstImageId, const std::string& secondImageId, const std::vector<cv::DMatch>& matches) {
		std::string outfilePath = matchFolder_ + std::to_string(currentMatchFileIndex_) + ".matches.txt";

		std::ofstream outfile;
		outfile.open(outfilePath, std::ios::app);

		outfile << firstImageId << " " << secondImageId << std::endl;
		outfile << "1" << std::endl;
		outfile << "sift_ocv " << matches.size() << std::endl;

		for (const auto& entry : matches) {
			outfile << entry.queryIdx << " " << entry.trainIdx << std::endl;
		}

		currentMatchFileLength_ += 3 + matches.size();
		if (currentMatchFileLength_ >= matchFileMaxLength_) {
			currentMatchFileIndex_++;
		}
	}

private:
	std::string featureFolder_;
	std::string matchFolder_;
	int currentMatchFileIndex_ = 0;
	int currentMatchFileLength_ = 0;
	int matchFileMaxLength_ = 500000;

};


#endif
