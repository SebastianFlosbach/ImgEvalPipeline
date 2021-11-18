#ifndef KEY_POINT_READER_HPP
#define KEY_POINT_READER_HPP


#include <fstream>
#include <map>
#include <string>

#include <opencv2/core.hpp>

class KeyPointReader {
public:
    KeyPointReader(const std::string& path) : path_(path) {}

    void readKeyPoints() {
        std::ifstream ifs;
        ifs.open(path_);

        if (!ifs.is_open()) {
            throw;
        }
        
        std::string line;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);

            float tmp;
            cv::KeyPoint source, target;
            iss >> source.pt.x >> source.pt.y >> source.size >> source.angle;
            iss >> tmp >> tmp >> tmp;
            iss >> target.pt.x >> target.pt.y >> target.size >> target.angle;

            std::string sourceName = std::to_string((int)source.pt.x) + "_" + std::to_string((int)source.pt.y);
            std::string targetName = std::to_string((int)target.pt.x) + "_" + std::to_string((int)target.pt.y);

            sourceKeyPoints_[sourceName] = source;
            targetKeyPoints_[targetName] = target;
        }
    }

    inline std::map<std::string, cv::KeyPoint> getSourceKeyPoints() const {
        return sourceKeyPoints_;
    }

    inline std::map<std::string, cv::KeyPoint> getTargetKeyPoints() const {
        return targetKeyPoints_;
    }

private:
   std::string path_;

   std::map<std::string, cv::KeyPoint> sourceKeyPoints_;
   std::map<std::string, cv::KeyPoint> targetKeyPoints_;

};


#endif
