#ifndef MATCH_DATA_HPP
#define MATCH_DATA_HPP


#include <vector>

#include <opencv2/core/types.hpp>

struct MatchData {
	int idImage1;
	int idImage2;
	std::vector<cv::DMatch> matches;
};


#endif
