#ifndef MATCHES_HPP
#define MATCHES_HPP


#include <string>
#include <vector>

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>

struct Matches {
	std::string source;
	std::string target;
	std::vector<cv::DMatch> matches;
};


#endif
