#ifndef ISERIALIZABLE_H_
#define ISERIALIZABLE_H_

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace NSTU
{
    class ISerializable
    {
        // Write serialization.
        virtual void write(cv::FileStorage& fileStorage) const = 0;

        // Read serialization.
        virtual void read(const cv::FileNode& fileNode) = 0;
    };
}

#endif