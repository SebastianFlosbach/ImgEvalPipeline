#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <string>

struct Config {
	std::string sfmDataPath;
	std::string matchFolder;
	std::string featureFolder;
	std::string imageMatches;
	double ransacThreshold;
	int minFeatures;
	bool outputMatches;
	std::string matchPath;
	bool outputFeatures;
	std::string featurePath;
};


#endif
