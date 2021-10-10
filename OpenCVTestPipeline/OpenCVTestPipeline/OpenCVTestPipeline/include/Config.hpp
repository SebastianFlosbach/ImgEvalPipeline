#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <string>

struct Config {
	std::string sfmDataPath;
	std::string outputPath;
	double ransacThreshold;
	int minFeatures;
	bool outputMatches;
	std::string matchPath;
	bool outputFeatures;
	std::string featurePath;
};


#endif
