#ifndef THREADED_FEATURE_EXTRACTOR_HPP
#define THREADED_FEATURE_EXTRACTOR_HPP


#include "ImageData.hpp"
#include "OpenCVSIFTFeatureExtractor.hpp"
#include "ThreadPool.hpp"
#include "ConsoleLogger.hpp"

#include <functional>
#include <iostream>
#include <string>

class ThreadedFeatureExtractor : public ThreadPool<ImageData, FeatureContainer> {
public:
	ThreadedFeatureExtractor(int minHessian) : 
		ThreadPool(std::bind(&ThreadedFeatureExtractor::extract, this, std::placeholders::_1)), 
		minHessian_(minHessian)
	{
	}

private:
	int minHessian_;

	FeatureContainer extract(const ImageData& imageData) {
		OpenCVSIFTFeatureExtractor extractor = OpenCVSIFTFeatureExtractor(minHessian_);

		auto features = extractor.calculateRegions(imageData);

		logFinish(std::string("Features: ") + std::to_string(features.keyPoints.size()));

		return features;
	}

};


#endif
