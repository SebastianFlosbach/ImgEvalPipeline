#ifndef OCV_SIFT_FEATURE_DETECTOR_HPP
#define OCV_SIFT_FEATURE_DETECTOR_HPP


#include "IFeatureDetector.hpp"
#include "Config.hpp"

#include <memory>
#include <vector>

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

namespace avf = aliceVision::feature;

class OcvSiftFeatureDetector : IFeatureDetector {
public:
	OcvSiftFeatureDetector(const Config& config) : config_(config) {
		detector_ = cv::SIFT::create(config_.minFeatures, 6);
	}

	FeatureContainer detectFeatures(const ImageData& imageData) override {
		std::vector<cv::KeyPoint> keyPoints;
		cv::Mat descriptors;

		detector_->detectAndCompute(imageData.image, cv::noArray(), keyPoints, descriptors);

		avf::SIFT_Regions* regions = new avf::SIFT_Regions();
		regions->Features().reserve(keyPoints.size());
		regions->Descriptors().reserve(keyPoints.size());

		cv::Mat siftsum;
		cv::reduce(descriptors, siftsum, 1, cv::REDUCE_SUM);

		int cpt = 0;
		for (const auto& entry : keyPoints) {
			avf::PointFeature feat(entry.pt.x, entry.pt.y, entry.size, entry.angle);
			regions->Features().push_back(feat);

			avf::Descriptor<unsigned char, 128> desc;
			for (std::size_t j = 0; j < 128; ++j)
			{
				desc[j] = static_cast<unsigned char>(512.0 * sqrt(descriptors.at<float>(cpt, j) / siftsum.at<float>(cpt, 0)));
			}
			regions->Descriptors().push_back(desc);
			++cpt;
		}

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
	cv::Ptr<cv::SIFT> detector_;

};


#endif
