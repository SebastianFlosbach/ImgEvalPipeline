#ifndef I_MATCHER_HPP
#define I_MATCHER_HPP


#include "FeatureDetection/FeatureContainer.hpp"
#include "FeatureMatching/MatchData.hpp"
#include "FeatureMatching/MatchingPair.hpp"

#include <map>
#include <memory>
#include <vector>

class IMatcher {
public:
	virtual std::vector<MatchData> match(const std::vector<MatchingPair>& pairs) = 0;
	virtual std::vector<MatchData> match(const std::vector<FeatureContainer>& features) = 0;
	virtual std::vector<MatchData> match(const std::vector<FeatureContainer>& features, const std::map<std::string, std::vector<std::string>>& imageMatches) = 0;
};
typedef std::shared_ptr<IMatcher> IMatcher_ptr;


#endif
