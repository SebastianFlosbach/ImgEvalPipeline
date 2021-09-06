#include "ImageData.hpp"
#include "OutputWriter.hpp"
#include "ThreadedFeatureExtractor.hpp"
#include "ThreadedMatcher.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <aliceVision/sfmData/SfMData.hpp>
#include <aliceVision/sfmDataIO/sfmDataIO.hpp>

#include <boost/program_options.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/xfeatures2d.hpp>

namespace avf = aliceVision::feature;
namespace po = boost::program_options;

int main(int argc, const char* argv[])
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

	std::string sfmDataPath;
	std::string outputPath;
	double ransacThreshold;
	int minFeatures;

	po::options_description params("Params");
	params.add_options()
		("help,h", "Produce help message.")
		("input,i", po::value<std::string>(&sfmDataPath)->required(), "SfMData file.")
		("output,o", po::value<std::string>(&outputPath)->required(), "Output path for the features and descriptors files (*.feat, *.desc).")
		("threshold,t", po::value<double>(&ransacThreshold)->default_value(2.0), "Threshold for RANSAC filtering.")
		("features,f", po::value<int>(&minFeatures)->default_value(400), "Number of features to retain.");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, params), vm);

	if (vm.count("help") || argc == 1) {
		std::cout << params << std::endl;
		return EXIT_SUCCESS;
	}

	vm.notify();

	aliceVision::sfmData::SfMData sfmData;

	std::cout << "Reading sfmData from " << sfmDataPath << std::endl;
	if (!aliceVision::sfmDataIO::Load(sfmData, sfmDataPath, aliceVision::sfmDataIO::ESfMData(aliceVision::sfmDataIO::VIEWS | aliceVision::sfmDataIO::INTRINSICS))) {
		std::cout << "Failed to load smfData from '" << sfmDataPath << "'!" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<ImageData> images;
	std::vector<FeatureContainer> features;

	std::cout << "Loading images" << std::endl;
	for (const auto& entry : sfmData.getViews()) {
		int imageId = entry.second.get()->getViewId();
		std::string imagePath = entry.second.get()->getImagePath();
		cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
		images.push_back({ imageId, image });
	}
	std::cout << "Found " << images.size() << " images" << std::endl;

	OutputWriter writer = OutputWriter(outputPath);
	writer.clearOutputDirectory();

	ThreadedFeatureExtractor extractor = ThreadedFeatureExtractor(minFeatures);
	for (const auto& image : images) {
		extractor.addInput(image);
	}
	extractor.run();
	extractor.wait();
	features = extractor.getResults();

	for (auto& feature : features) {
		writer.writeRegions(feature.imageId, feature.regions);
	}

	ThreadedMatcher matcher = ThreadedMatcher(ransacThreshold, features);
	matcher.run();
	matcher.wait();
	std::vector<MatchData> matchData = matcher.getResults();

	for (const auto& match : matchData) {
		writer.writeMatches(std::to_string(match.idImage1), std::to_string(match.idImage2), match.matches);
	}

	return EXIT_SUCCESS;
}
