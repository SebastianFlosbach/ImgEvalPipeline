#ifndef FLANN_MATCHER_HPP
#define FLANN_MATCHER_HPP


#include "FeatureMatching/IMatcher.hpp"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <vector>

class FlannMatcher : public IMatcher {
public:
	std::vector<MatchData> match(const std::vector<MatchingPair>& pairs) override {
		cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector<std::vector<cv::DMatch>> knn_matches;
		std::vector<MatchData> matches;

		for (auto pair : pairs) {
			matcher->knnMatch(pair.image1.descriptors, pair.image2.descriptors, knn_matches, 2);

			const float ratio_thresh = 0.7f;
			std::vector<cv::DMatch> good_matches;
			for (size_t i = 0; i < knn_matches.size(); i++)
			{
				if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
				{
					good_matches.push_back(knn_matches[i][0]);
				}
			}

			matches.push_back({pair.image1.imageId, pair.image2.imageId, good_matches });
		}
		
		return matches;
	}

	std::vector<MatchData> match(const std::vector<FeatureContainer>& features) override {
		cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector<std::vector<cv::DMatch>> knn_matches;
		std::vector<MatchData> matches;

		for (size_t indexImage1 = 0; indexImage1 < features.size() - 1; indexImage1++) {
			for (size_t indexImage2 = indexImage1 + 1; indexImage2 < features.size(); indexImage2++) {
				matcher->knnMatch(features[indexImage1].descriptors, features[indexImage2].descriptors, knn_matches, 2);

				const float ratio_thresh = 0.7f;
				std::vector<cv::DMatch> good_matches;
				for (size_t i = 0; i < knn_matches.size(); i++)
				{
					if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
					{
						good_matches.push_back(knn_matches[i][0]);
					}
				}

				matches.push_back({features[indexImage1].imageId, features[indexImage2].imageId, good_matches});
			}
		}

		return matches;
	}

};


#endif
