#ifndef I_FEATURE_DETECTOR_HPP
#define I_FEATURE_DETECTOR_HPP


#include "FeatureContainer.hpp"
#include "ImageData.hpp"

#include <memory>

class IFeatureDetector {
public:
	virtual FeatureContainer detectFeatures(const ImageData& imageData) = 0;
};
typedef std::shared_ptr<IFeatureDetector> IFeatureDetector_ptr;


#endif
