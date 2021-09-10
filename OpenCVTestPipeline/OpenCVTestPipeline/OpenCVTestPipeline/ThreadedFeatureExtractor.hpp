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
		minHessian_(minHessian),
		logger_(ConsoleLogger::instance())
	{
	}

private:
	int minHessian_;
	ConsoleLogger* logger_;

	FeatureContainer extract(const ImageData& imageData) {
		OpenCVSIFTFeatureExtractor extractor = OpenCVSIFTFeatureExtractor(minHessian_);

		auto features = extractor.calculateRegions(imageData);

		logger_->log("Feature extraction " + std::to_string(getResultCount() + 1) + "/" + std::to_string(getInputCount()) + " done.");

		return features;
	}

};


#endif
