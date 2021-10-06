#ifndef MATCHER_HPP
#define MATCHER_HPP


#include "ImageData.hpp"
#include "MatchData.hpp"
#include "FeatureContainer.hpp"

#include <iostream>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

class Matcher {
public:
	Matcher(double ransacThreshold) : ransacThreshold_(ransacThreshold)
	{
	}

	std::vector<MatchData> match(const std::vector<FeatureContainer>& features) {
		std::vector<MatchData> matchData;

		cv::Mat fundamentalMat, mask;
		cv::BFMatcher matcher;

		float maxMatchings = 0.5f * (float)(features.size() - 1) * (float)features.size();
		float currentMatching = 0.0f;
		int matchCount = 0;
		std::cout << "Matching " << features.size() << " images for " << (int)maxMatchings << " matchings" << std::endl;
		for (size_t firstImageIndex = 0; firstImageIndex < features.size() - 1; firstImageIndex++) {
			auto& descriptorFirstImage = features.at(firstImageIndex).descriptors;
			auto firstImageId = features.at(firstImageIndex).imageId;

			for (size_t secondImageIndex = firstImageIndex + 1; secondImageIndex < features.size(); secondImageIndex++) {
				std::cout << currentMatching / maxMatchings * 100 << "% done" << std::endl;
				currentMatching++;

				std::vector<cv::DMatch> matches;
				std::vector<cv::DMatch> ransac_matches;

				auto& descriptorSecondImage = features.at(secondImageIndex).descriptors;
				auto secondImageId = features.at(secondImageIndex).imageId;

				matcher.match(descriptorFirstImage, descriptorSecondImage, matches);

				std::vector<cv::Point2f> imagePoints_1;
				std::vector<cv::Point2f> imagePoints_2;

				getImagePoints(matches, features.at(firstImageIndex).keyPoints, features.at(secondImageIndex).keyPoints, imagePoints_1, imagePoints_2);

				fundamentalMat = cv::findFundamentalMat(imagePoints_1, imagePoints_2, cv::RANSAC, ransacThreshold_, 0.99, mask);

				for (int j = 0; j < (int)matches.size(); j++) {
					if ((unsigned int)mask.at<uchar>(j, 0) > 0)
					{
						ransac_matches.push_back(matches[j]);
					}
				}

				matchData.push_back({ firstImageId, secondImageId, ransac_matches });
				matchCount += ransac_matches.size();
			}
		}

		std::cout << "Found an average of " << (int)(matchCount / maxMatchings) << " matchings" << std::endl;
		return matchData;
	}

private:
	double ransacThreshold_;

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
