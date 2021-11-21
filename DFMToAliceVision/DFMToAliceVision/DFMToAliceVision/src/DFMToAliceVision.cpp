#include <filesystem>
#include <iostream>
#include <map>

#include "DescriptorReader.hpp"
#include "KeyPointReader.hpp"
#include "MatchReader.hpp"
#include "ImageData.hpp"
#include "OutputWriter.hpp"
#include "Matches.hpp"

#include <aliceVision/sfmData/SfMData.hpp>
#include <aliceVision/sfmDataIO/sfmDataIO.hpp>
#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <boost/program_options.hpp>

namespace avf = aliceVision::feature;
namespace po = boost::program_options;

std::string getSourceImageName(const std::string& name) {
	if (name.rfind("Descriptor", 0) == 0) {
		return name.substr(10, name.find('-') - 10);
	}
	if (name.rfind("KP", 0) == 0 || name.rfind("DT", 0) == 0) {
		return name.substr(2, name.find('-') - 2);
	}
	return name.substr(0, name.find('-'));
}

std::string getTargetImageName(const std::string& name) {
	if (name.rfind("Descriptor", 0) == 0) {
		return name.substr(name.find('-') + 1, name.find('_') - name.find('-') - 1);
	}
	if (name.rfind("KP", 0) == 0 || name.rfind("DT", 0) == 0) {
		return name.substr(name.find('-') + 1, name.find('.') - name.find('-') - 1);
	}
	return name.substr(name.find('-') + 1);
}

void getImageNames(const std::string& name, std::string& first, std::string& second) {
	if (name.rfind("Descriptor", 0) == 0) {
		first = name.substr(10, name.find('-'));
		second = name.substr(name.find('-') + 1, name.find('_'));
		return;
	}
	if (name.rfind("KP", 0) == 0 || name.rfind("DT", 0) == 0) {
		first = name.substr(2, name.find('-'));
		second = name.substr(name.find('-') + 1, name.find('.'));
		return;
	}
	first = name.substr(0, name.find('-'));
	second = name.substr(name.find('-') + 1);
}

int main(int argc, char* argv[])
{
	std::string output;
	std::string dfmPath;
	std::string sfmDataPath;
	bool printMatches;
	bool useOutlierFiltering;
	float outlierThreshold;
	bool useKeyPointFiltering;
	int keyPointThreshold;
	int maxFileLength;

	po::options_description params("Params");
	params.add_options()
		("help,h", "Produce help message.")
		("output,o", po::value<std::string>(&output)->required(), "Output path for the features and descriptors files (*.feat, *.desc).")
		("dfmPath,d", po::value<std::string>(&dfmPath)->required(), "Path to folder with dfm feature data")
		("sfmFile,s", po::value<std::string>(&sfmDataPath)->required(), "Path to sfm file from AliceVision CameraInit.")
		("printMatches,pm", po::value<bool>(&printMatches)->default_value(false), "Show matches.")
		("useOutlierFiltering", po::value<bool>(&useOutlierFiltering)->default_value(false))
		("outlierThreshold", po::value<float>(&outlierThreshold)->default_value(0.75), "Set threshold for outlier detection.")
		("useKeyPointFiltering", po::value<bool>(&useKeyPointFiltering)->default_value(false))
		("keyPointThreshold", po::value<int>(&keyPointThreshold)->default_value(10))
		("maxFileLength", po::value<int>(&maxFileLength)->default_value(200000));
		

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, params), vm);

	if (vm.count("help") || argc == 1) {
		std::cout << params << std::endl;
		return EXIT_SUCCESS;
	}

	vm.notify();

	OutputWriter writer = OutputWriter(output, maxFileLength);
	writer.createOutputDirectory();

	aliceVision::sfmData::SfMData sfmData;
	std::cout << "Reading sfmData from " << sfmDataPath << std::endl;
	auto esfmData = aliceVision::sfmDataIO::ESfMData(aliceVision::sfmDataIO::VIEWS);
	if (!aliceVision::sfmDataIO::Load(sfmData, sfmDataPath, esfmData)) {
		std::cout << "Failed to load sfmData from '" << sfmDataPath << "'!" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Reading image entries" << std::endl;
	std::map<std::string, ImageData> images;
	for (const auto& entry : sfmData.getViews()) {
		std::filesystem::path imagePath(entry.second.get()->getImagePath());
		std::string imageId = std::to_string(entry.second.get()->getViewId());
		images[imagePath.stem().string()] = ImageData(imageId, imagePath.stem().string());
		//std::cout << imagePath.stem().string() << " <-> " << imageId << std::endl;

		cv::Mat image = cv::imread(entry.second.get()->getImagePath(), cv::IMREAD_COLOR);
		images[imagePath.stem().string()].addImage(image);
	}

	std::cout << "Reading KeyPoints" << std::endl;
	for (const auto& entry : std::filesystem::directory_iterator(dfmPath)) {
		if (entry.path().stem().string().rfind("KP", 0) != 0) continue;

		std::string sourceName = getSourceImageName(entry.path().stem().string());
		std::string targetName = getTargetImageName(entry.path().stem().string());

		if (images.find(sourceName) == images.end()) {
			std::cout << "Couldn't find source image " << sourceName << " in sfm data!" << std::endl;
			continue;
		}

		if (images.find(targetName) == images.end()) {
			std::cout << "Couldn't find target image " << targetName << " in sfm data!" << std::endl;
			continue;
		}

		//std::cout << entry.path().stem().string() << std::endl;

		KeyPointReader reader = KeyPointReader(entry.path().string());
		reader.readKeyPoints();

		images[sourceName].addKeyPoints(reader.getSourceKeyPoints());
		images[targetName].addKeyPoints(reader.getTargetKeyPoints());
	}

	for (const auto& image : images) {
		std::vector<cv::KeyPoint> keyPoints = image.second.getKeyPoints();

		std::cout << "Image " << image.second.getName() << ": " << keyPoints.size() << " KeyPoints" << std::endl;

		avf::SIFT_Regions* imageRegions = new avf::SIFT_Regions();
		imageRegions->Features().reserve(keyPoints.size());
		imageRegions->Descriptors().reserve(keyPoints.size());

		for (const auto& kp : keyPoints) {
			avf::PointFeature feat(kp.pt.x, kp.pt.y, kp.size, kp.angle);
			imageRegions->Features().push_back(feat);
		}

		writer.writeRegions( image.second.getId(), std::unique_ptr<avf::SIFT_Regions>(imageRegions)); 
	}

	std::map<int, std::vector<Matches>> bags;

	std::cout << "Reading matches" << std::endl;
	for (const auto& entry : std::filesystem::directory_iterator(dfmPath)) {
		if (entry.path().extension() != ".txt") continue;
		if (entry.path().filename() == "relations.txt") continue;
		if (entry.path().stem().string().rfind("DT", 0) == 0) continue;
		if (entry.path().stem().string().rfind("KP", 0) == 0) continue;

		std::string sourceName = getSourceImageName(entry.path().stem().string());
		std::string targetName = getTargetImageName(entry.path().stem().string());

		if (images.find(sourceName) == images.end()) {
			std::cout << "Couldn't find source image " << sourceName << " in sfm data!" << std::endl;
			continue;
		}

		if (images.find(targetName) == images.end()) {
			std::cout << "Couldn't find target image " << targetName << " in sfm data!" << std::endl;
			continue;
		}

		MatchReader reader = MatchReader(entry.path().string(), keyPointThreshold);
		std::vector<cv::DMatch> matches = reader.readMatches(images[sourceName].getKeyPointMap(), images[targetName].getKeyPointMap());

		// KeyPoint filtering
		if (useKeyPointFiltering && !reader.isMatchListValid()) {
			std::cout << "Discarding invalid pair " << sourceName << " - " << targetName << std::endl;
			continue;
		}

		Matches m = {
			sourceName,
			targetName,
			matches
		};

		if (useOutlierFiltering) {
			// Outlier detection
			int nbMatches = matches.size();
			bool foundBag = false;

			for (auto pair : bags) {
				float upperBound = pair.first * (1 + outlierThreshold);
				float lowerBound = pair.first * (1 - outlierThreshold);

				if (nbMatches <= upperBound && nbMatches >= lowerBound) {
					std::cout << "Adding " << nbMatches << " to bag " << pair.first << std::endl;
					bags[pair.first].push_back(m);
					foundBag = true;
				}
			}

			if (!foundBag) {
				std::cout << "Creating new bag with " << nbMatches << " matches" << std::endl;
				bags[nbMatches].push_back(m);
			}
		}
		else {
			bags[0].push_back(m);
		}

	}

	int bestBagValue = bags.begin()->first;
	std::vector<Matches> bestBag = bags.begin()->second;
	if (useOutlierFiltering) {
		for (auto& bag : bags) {
			if (bag.second.size() > bestBag.size()) {
				bestBagValue = bag.first;
				bestBag = bag.second;
			}
		}
		std::cout << "Best bag is " << bestBagValue << " with size " << bestBag.size() << std::endl;
	}

	std::cout << "Writing matches" << std::endl;
	for (auto& matchData : bestBag) {
		if (printMatches) {
			cv::Mat image;
			cv::drawMatches(
				images[matchData.source].getImage(), 
				images[matchData.source].getKeyPoints(), 
				images[matchData.target].getImage(), 
				images[matchData.target].getKeyPoints(),
				matchData.matches,
				image,
				cv::Scalar::all(-1), 
				cv::Scalar::all(-1), 
				std::vector<char>(), 
				cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
			);

			cv::imwrite("matchVisualisation/" + matchData.source + "_" + matchData.target + ".png", image);
			cv::waitKey();
		}

		std::cout << "Pair " << matchData.source << " - " << matchData.target << ": " << matchData.matches.size() << " matches" << std::endl;

		writer.writeMatches(std::to_string(images[matchData.source].getId()), std::to_string(images[matchData.target].getId()), matchData.matches);
	}

	return EXIT_SUCCESS;
}
