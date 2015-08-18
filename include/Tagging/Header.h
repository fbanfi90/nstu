#pragma once

// Standard C++.
#include <iostream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"
#include "ISerializable.h"
#include "DateTime.h"
#include "Location.h"

namespace NSTU {
namespace Tagging {

class Header : ISerializable
{
public:

    // Default constructor.
    Header();

    // Location constructor.
    Header(std::string fileName, DateTime dateTime, Location location);

    // Write serialization.
    void write(cv::FileStorage& fileStorage) const;

    // Read serialization.
    void read(const cv::FileNode& fileNode);

    // Out stream operator.
    friend std::ostream& operator<<(std::ostream& stream, const Header& header);

public:

    // Location information.
    std::string fileName;
    DateTime dateTime;
    Location location;
};

// Must be defined for the serialization in FileStorage to work.
static void write(cv::FileStorage& fileStorage, const std::string&, const Header& header)
{
    header.write(fileStorage);
}

// Must be defined for the serialization in FileStorage to work.
static void read(const cv::FileNode& fileNode, Header& header, const Header& defaultValue = Header())
{
    if (fileNode.empty())
        header = defaultValue;
    else
        header.read(fileNode);
}

} // namespace Tagging
} // namespace NSTU