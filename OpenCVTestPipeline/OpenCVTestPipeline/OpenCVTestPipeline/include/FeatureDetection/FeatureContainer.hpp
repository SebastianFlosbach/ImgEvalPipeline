#ifndef FEATURE_CONTAINER_HPP
#define FEATURE_CONTAINER_HPP


#include <memory>
#include <vector>

#include <opencv2/features2d.hpp>

#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

namespace avf = aliceVision::feature;

struct FeatureContainer {
	int imageId;
	std::unique_ptr<avf::SIFT_Regions> regions;
	std::vector<cv::KeyPoint> keyPoints;
	cv::Mat descriptors;
};


#endif
