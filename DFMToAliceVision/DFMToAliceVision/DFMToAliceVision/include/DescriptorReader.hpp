#ifndef DESCRIPTOR_READER_HPP
#define DESCRIPTOR_READER_HPP


#include <string>
#include <fstream>

#include <opencv2/core.hpp>

class DescriptorReader {
public:
    DescriptorReader(const std::string& path) : path_(path) {}

    cv::Mat readMatBinary()
    {
        cv::Mat descriptor;

        std::ifstream ifs;
        ifs.open(path_);

        if (!ifs.is_open()) {
            throw;
        }
        int rows, cols, type;
        ifs.read((char*)(&rows), sizeof(int));
        if (rows == 0) {
            throw;
        }
        ifs.read((char*)(&cols), sizeof(int));
        ifs.read((char*)(&type), sizeof(int));
        descriptor.release();
        descriptor.create(rows, cols, type);
        ifs.read((char*)(descriptor.data), descriptor.elemSize() * descriptor.total());
        return descriptor;
    }

private:
    std::string path_;

};


#endif
