#ifndef OPENCV_SIFT_FEATURE_EXTRACTOR_HPP
#define OPENCV_SIFT_FEATURE_EXTRACTOR_HPP


#include "FeatureContainer.hpp"
#include "ImageData.hpp"

#include <memory>
#include <vector>

#include <opencv2/features2d.hpp>

#include <aliceVision/image/all.hpp>
#include <aliceVision/feature/imageDescriberCommon.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/feature/regionsFactory.hpp>

namespace avf = aliceVision::feature;

class OpenCVSIFTFeatureExtractor {
public:
	OpenCVSIFTFeatureExtractor(int minHessian) : minHessian_( minHessian ) {
		detector_ = cv::SIFT::create(minHessian);
	}

	FeatureContainer calculateRegions(const ImageData& imageData) {
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

		return { imageData.id, std::unique_ptr<avf::SIFT_Regions>(regions), keyPoints, descriptors };
	}

private:
	int minHessian_;
	cv::Ptr<cv::SIFT> detector_;

};


#endif
