#ifndef IMAGE_DATA_HPP
#define IMAGE_DATA_HPP


#include <map>
#include <string>

class ImageData {
public:
	ImageData() {}
	ImageData(std::string id, std::string name) : id_(std::stoi(id)), name_(name) {}

	void addImage(const cv::Mat& image) {
		image_ = image;
	}

	void addKeyPoints(const std::map<std::string, cv::KeyPoint>& keyPoints) {
		for (const auto& entry : keyPoints) {
			keyPoints_[entry.first] = entry.second;
		}
	}

	void addDescriptor(const cv::Mat& descriptor) {
		descriptor_ = descriptor;
	}

	int getId() const {
		return id_;
	}

	std::string getName() const {
		return name_;
	}

	std::vector<cv::KeyPoint> getKeyPoints() const {
		std::vector<cv::KeyPoint> keyPoints;
		std::transform(keyPoints_.begin(), keyPoints_.end(), std::back_inserter(keyPoints), [](auto& kv) { return kv.second; });
		return keyPoints;
	}

	std::map<std::string, cv::KeyPoint>& getKeyPointMap() {
		return keyPoints_;
	}

	const cv::Mat& getDescriptor() const {
		return descriptor_;
	}

	const cv::Mat& getImage() const {
		return image_;
	}

private:
	int id_;
	std::string name_;
	std::map<std::string, cv::KeyPoint> keyPoints_;
	cv::Mat descriptor_;
	cv::Mat image_;

};


#endif
