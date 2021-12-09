#include "ImageData.hpp"
#include "OutputWriter.hpp"
#include "FeatureDetection/ThreadedFeatureDetector.hpp"
#include "FeatureMatching/ThreadedMatcher.hpp"
#include "FeatureMatching/FlannMatcher.hpp"
#include "FeatureMatching/IMatcher.hpp"
#include "Config.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
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

	Config config;

	po::options_description params("Params");
	params.add_options()
		("help,h", "Produce help message.")
		("input,i", po::value<std::string>(&config.sfmDataPath)->required(), "SfMData file.")
		("featureFolder", po::value<std::string>(&config.featureFolder)->required(), "Output folder for the features and descriptors files (*.feat, *.desc).")
		("matchFolder", po::value<std::string>(&config.matchFolder)->required(), "Output folder for match files.")
		("imageMatches", po::value<std::string>(&config.imageMatches)->default_value(""), "File with image matches to use for feature matching.")
		("threshold,t", po::value<double>(&config.ransacThreshold)->default_value(2.0), "Threshold for RANSAC filtering.")
		("features,f", po::value<int>(&config.minFeatures)->default_value(5000), "Number of features to retain.")
		("outputMatches,om", po::value<bool>(&config.outputMatches)->default_value(false), "Enable output of match visualisation.")
		("matchPath,mp", po::value<std::string>(&config.matchPath)->default_value(""), "Where to safe match images.")
		("outputFeatures,of", po::value<bool>(&config.outputFeatures)->default_value(false), "Enable output of feature visualisation.")
		("featurePath,fp", po::value<std::string>(&config.featurePath)->default_value(""), "Where to safe feature images.");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, params), vm);

	if (vm.count("help") || argc == 1) {
		std::cout << params << std::endl;
		return EXIT_SUCCESS;
	}

	vm.notify();

	aliceVision::sfmData::SfMData sfmData;

	std::cout << "Reading sfmData from " << config.sfmDataPath << std::endl;
	if (!aliceVision::sfmDataIO::Load(sfmData, config.sfmDataPath, aliceVision::sfmDataIO::ESfMData(aliceVision::sfmDataIO::VIEWS | aliceVision::sfmDataIO::INTRINSICS))) {
		std::cout << "Failed to load sfmData from '" << config.sfmDataPath << "'!" << std::endl;
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

	OutputWriter writer = OutputWriter(config.featureFolder, config.matchFolder);
	writer.clearOutputDirectory();

	std::cout << "Extracting features" << std::endl;
	ThreadedFeatureDetector extractor = ThreadedFeatureDetector(config);
	for (const auto& image : images) {
		extractor.addInput(image);
	}
	extractor.run();
	extractor.wait();
	features = extractor.getResults();

	for (auto& feature : features) {
		writer.writeRegions(feature.imageId, feature.regions);
	}

	std::map<std::string, std::vector<std::string>> imageMatches;
	if (!config.imageMatches.empty()) {
		std::ifstream inputStream;
		inputStream.open(config.imageMatches);

		std::string line;
		while (std::getline(inputStream, line)) {
			std::istringstream iss(line);
			std::string word;
			std::string source = "";
			while (std::getline(iss, word, ' ')) {
				if (source == "") {
					source = word;
				}
				else {
					imageMatches[source].push_back(word);
				}
			}
		}
	}

	std::vector<MatchData> matchData = std::vector<MatchData>();
	std::cout << "Calculating matches" << std::endl;
	auto matcher = std::unique_ptr<IMatcher>(new ThreadedMatcher(config.ransacThreshold));
	//IMatcher_ptr matcher = std::unique_ptr<IMatcher>(new FlannMatcher());
	if (config.imageMatches.empty()) {
		matchData = matcher->match(features);
	}
	else {
		matchData = matcher->match(features, imageMatches);
	}

	for (const auto& match : matchData) {
		writer.writeMatches(std::to_string(match.idImage1), std::to_string(match.idImage2), match.matches);	

		if (config.outputMatches) {
			int idImage1 = match.idImage1;
			int idImage2 = match.idImage2;
			auto itImg1 = std::find_if(images.begin(), images.end(), [idImage1](const ImageData& image) { return image.id == idImage1; });
			auto itImg2 = std::find_if(images.begin(), images.end(), [idImage2](const ImageData& image) { return image.id == idImage2; });
			auto itFtr1 = std::find_if(features.begin(), features.end(), [idImage1](const FeatureContainer& feature) { return feature.imageId == idImage1; });
			auto itFtr2 = std::find_if(features.begin(), features.end(), [idImage2](const FeatureContainer& feature) { return feature.imageId == idImage2; });

			cv::Mat matchImage;
			cv::drawMatches(itImg1->image, itFtr1->keyPoints, itImg2->image, itFtr2->keyPoints, match.matches, matchImage, 5);
			std::string fileName = config.matchPath + "match_" + std::to_string(idImage1) + "_" + std::to_string(idImage2) + ".jpg";
			cv::imwrite(fileName, matchImage);
		}
	}

	return EXIT_SUCCESS;
}
