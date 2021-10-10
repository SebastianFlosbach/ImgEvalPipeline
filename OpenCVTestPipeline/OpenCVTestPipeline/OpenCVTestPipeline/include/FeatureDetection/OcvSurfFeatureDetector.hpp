#ifndef OCV_SURF_FEATURE_DETECTOR_HPP
#define OCV_SURF_FEATURE_DETECTOR_HPP


#include "IFeatureDetector.hpp"
#include "Config.hpp"

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

class OcvSurfFeatureDetector : IFeatureDetector {
public:
	OcvSurfFeatureDetector(const Config& config) : config_(config) {
		detector_ = cv::xfeatures2d::SURF::create(config.minFeatures);
	}

	FeatureContainer detectFeatures(const ImageData& imageData) override {
		std::vector<cv::KeyPoint> keyPoints;
		cv::Mat descriptors;

		detector_->detectAndCompute(imageData.image, cv::noArray(), keyPoints, descriptors);

		avf::SIFT_Regions* regions = new avf::SIFT_Regions();

		if (config_.outputFeatures) {
			cv::Mat featureImg;
			cv::drawKeypoints(imageData.image, keyPoints, featureImg, cv::Scalar(0, 240, 0));
			std::string fileName = config_.featurePath + "feature_" + std::to_string(imageData.id) + ".jpg";
			cv::imwrite(fileName, featureImg);
		}

		return { imageData.id, std::unique_ptr<avf::SIFT_Regions>(regions), keyPoints, descriptors };
	}

private:
	Config config_;
	cv::Ptr<cv::xfeatures2d::SURF> detector_;
};


#endif
