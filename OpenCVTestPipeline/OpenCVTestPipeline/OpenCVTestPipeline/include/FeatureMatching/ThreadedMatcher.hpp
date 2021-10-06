#ifndef THREADED_MATCHER_HPP
#define THREADED_MATCHER_HPP


#include "FeatureDetection/FeatureContainer.hpp"
#include "FeatureMatching/MatchData.hpp"
#include "FeatureMatching/MatchingPair.hpp"
#include "ThreadPool.hpp"
#include "ConsoleLogger.hpp"
#include "FeatureMatching/IMatcher.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

#include <functional>
#include <iostream>
#include <vector>

class ThreadedMatcher : public IMatcher, private ThreadPool<MatchingPair, MatchData> {
public:
	ThreadedMatcher(double ransacThreshold);

	std::vector<MatchData> match(const std::vector<MatchingPair>& pairs) override;
	std::vector<MatchData> match(const std::vector<FeatureContainer>& features) override;

private:
	double ransacThreshold_;

	MatchData work(const MatchingPair& pair);

	void getImagePoints(std::vector<cv::DMatch>& good_matches, const std::vector<cv::KeyPoint>& keypoints_1, const std::vector<cv::KeyPoint>& keypoints_2, std::vector<cv::Point2f>& ImagePoints_1, std::vector<cv::Point2f>& ImagePoints_2);

};


#endif
