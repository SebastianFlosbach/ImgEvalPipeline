#ifndef MATCH_READER_HPP
#define MATCH_READER_HPP


#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

class MatchReader {
public:
	MatchReader(const std::string& path, int keyPointThreshold) : path_(path), threshold_(keyPointThreshold) {}

	std::vector<cv::DMatch> readMatches(const std::map<std::string, cv::KeyPoint>& kpImage1, const std::map<std::string, cv::KeyPoint>& kpImage2) {
        std::ifstream ifs;
        ifs.open(path_);

        if (!ifs.is_open()) {
            throw;
        }

        std::vector<cv::DMatch> matches;

        std::map<int, int> indexCounter;

        isValid_ = true;
        std::string line;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);

            std::string kp11, kp12, kp21, kp22;
            iss >> kp11 >> kp12;
            iss >> kp21 >> kp22;

            std::string kp1 = kp11 + "_" + kp12;
            std::string kp2 = kp21 + "_" + kp22;

            if (kpImage1.find(kp1) == kpImage1.end()) {
                std::cout << "Couldn't find " << kp1 << std::endl;
                continue;
            }
            if (kpImage2.find(kp2) == kpImage2.end()) {
                std::cout << "Couldn't find " << kp2 << std::endl;
                continue;
            }

            int indexKP1 = std::distance(kpImage1.begin(), kpImage1.find(kp1));
            int indexKP2 = std::distance(kpImage2.begin(), kpImage2.find(kp2));

            if (indexCounter.find(indexKP2) == indexCounter.end()) {
                indexCounter[indexKP2] = 1;
            }
            else {
                indexCounter[indexKP2] += 1;
            }

            matches.push_back(cv::DMatch(indexKP1, indexKP2, 0.0));
        }

        for (auto index : indexCounter) {
            if (index.second > threshold_) {
                isValid_ = false;
                break;
            }
        }

        return matches;
	}

    inline bool isMatchListValid() const {
        return isValid_;
    }

private:
	std::string path_;
    bool isValid_ = true;
    int threshold_;

};


#endif
