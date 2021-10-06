#ifndef I_MATCHER_HPP
#define I_MATCHER_HPP


#include "FeatureDetection/FeatureContainer.hpp"
#include "FeatureMatching/MatchData.hpp"
#include "FeatureMatching/MatchingPair.hpp"

#include <memory>
#include <vector>

class IMatcher {
public:
	virtual std::vector<MatchData> match(const std::vector<MatchingPair>& pairs) = 0;
	virtual std::vector<MatchData> match(const std::vector<FeatureContainer>& features) = 0;

};
typedef std::unique_ptr<IMatcher> IMatcher_ptr


#endif
