#ifndef MATCHING_PAIR_HPP
#define MATCHING_PAIR_HPP


#include "FeatureDetection/FeatureContainer.hpp"

struct MatchingPair {
	const FeatureContainer& image1;
	const FeatureContainer& image2;
};


#endif
