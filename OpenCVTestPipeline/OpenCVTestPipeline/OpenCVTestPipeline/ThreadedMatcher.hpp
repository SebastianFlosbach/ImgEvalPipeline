#ifndef THREADED_MATCHER_HPP
#define THREADED_MATCHER_HPP


#include "FeatureContainer.hpp"
#include "MatchData.hpp"
#include "MatchingPair.hpp"
#include "ThreadPool.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

#include <functional>
#include <iostream>
#include <vector>

class ThreadedMatcher : public ThreadPool<MatchingPair, MatchData> {
public:
	ThreadedMatcher(double ransacThreshold, const std::vector<FeatureContainer>& features) : 
		ThreadPool(std::bind(&ThreadedMatcher::match, this, std::placeholders::_1)), 
		ransacThreshold_(ransacThreshold)
	{
		for (size_t indexImage1 = 0; indexImage1 < features.size() - 1; indexImage1++) {
			for (size_t indexImage2 = indexImage1 + 1; indexImage2 < features.size(); indexImage2++) {
				addInput({ features.at(indexImage1), features.at(indexImage2) });
			}
		}
	}

private:
	double ransacThreshold_;

	MatchData match(const MatchingPair& pair) {
		cv::Mat fundamentalMat, mask;
		cv::BFMatcher matcher;

		std::vector<cv::DMatch> matches;
		std::vector<cv::DMatch> ransac_matches;

		auto& descriptorsImage1 = pair.image1.descriptors;
		auto idImage1 = pair.image1.imageId;

		auto& descriptorsImage2 = pair.image2.descriptors;
		auto idImage2 = pair.image2.imageId;

		matcher.match(descriptorsImage1, descriptorsImage2, matches);

		std::vector<cv::Point2f> imagePoints_1;
		std::vector<cv::Point2f> imagePoints_2;

		getImagePoints(matches, pair.image1.keyPoints, pair.image2.keyPoints, imagePoints_1, imagePoints_2);

		fundamentalMat = cv::findFundamentalMat(imagePoints_1, imagePoints_2, cv::RANSAC, ransacThreshold_, 0.99, mask);

		for (int j = 0; j < (int)matches.size(); j++) {
			if ((unsigned int)mask.at<uchar>(j, 0) > 0)
			{
				ransac_matches.push_back(matches[j]);
			}
		}

		std::cout << "Match " << getResultCount() + 1 << "/" << getInputCount() << " finished." << std::endl;
		return { idImage1, idImage2, ransac_matches };
	}

	void getImagePoints(std::vector<cv::DMatch>& good_matches, const std::vector<cv::KeyPoint>& keypoints_1, const std::vector<cv::KeyPoint>& keypoints_2, std::vector<cv::Point2f>& ImagePoints_1, std::vector<cv::Point2f>& ImagePoints_2)
	{
		for (size_t i = 0; i < good_matches.size(); i++)
		{
			cv::Point2f point1 = keypoints_1[good_matches[i].queryIdx].pt;
			cv::Point2f point2 = keypoints_2[good_matches[i].trainIdx].pt;
			ImagePoints_1.push_back(point1);
			ImagePoints_2.push_back(point2);
		}
	}

};


#endif
