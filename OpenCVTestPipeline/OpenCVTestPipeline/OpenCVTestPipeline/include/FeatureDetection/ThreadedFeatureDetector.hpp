#ifndef THREADED_FEATURE_DETECTOR_HPP
#define THREADED_FEATURE_DETECTOR_HPP


#include "ImageData.hpp"
#include "OcvSurfFeatureDetector.hpp"
#include "OcvSiftFeatureDetector.hpp"
#include "OcvFastFeatureDetector.hpp"
#include "ThreadPool.hpp"
#include "ConsoleLogger.hpp"
#include "Config.hpp"

#include <functional>
#include <iostream>
#include <string>

class ThreadedFeatureDetector : public ThreadPool<ImageData, FeatureContainer> {
public:
	ThreadedFeatureDetector(const Config& config) :
		ThreadPool(std::bind(&ThreadedFeatureDetector::extract, this, std::placeholders::_1)), 
		config_(config)
	{
	}

private:
	Config config_;

	FeatureContainer extract(const ImageData& imageData) {
		OcvSiftFeatureDetector detector = OcvSiftFeatureDetector(config_);
		//OcvSurfFeatureDetector detector = OcvSurfFeatureDetector(config_);
		//OcvFastFeatureDetector detector = OcvFastFeatureDetector(config_);

		auto features = detector.detectFeatures(imageData);

		logFinish(std::string("Features: ") + std::to_string(features.keyPoints.size()));

		return features;
	}

};


#endif
