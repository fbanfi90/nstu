#pragma once

// Standard C++.
#include <iostream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"
#include "ISerializable.h"

namespace NSTU {
namespace Tagging {

class DateTime : ISerializable
{
public:

    // Default constructor.
    DateTime();

    // Date and time constructor.
    DateTime(int day, int month, int year, int hour, int minute, int second);

    // The DateTime object representing current date and time.
    static DateTime now();

    // Write serialization.
    void write(cv::FileStorage& fileStorage) const;

    // Read serialization.
    void read(const cv::FileNode& fileNode);

    // Out stream operator.
    friend std::ostream& operator<<(std::ostream& stream, const DateTime& dateTime);

public:

    // Date and time information.
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int second;
};

// Must be defined for the serialization in FileStorage to work.
static void write(cv::FileStorage& fileStorage, const std::string&, const DateTime& dateTime)
{
    dateTime.write(fileStorage);
}

// Must be defined for the serialization in FileStorage to work.
static void read(const cv::FileNode& fileNode, DateTime& dateTime, const DateTime& defaultValue = DateTime())
{
    if (fileNode.empty())
        dateTime = defaultValue;
    else
        dateTime.read(fileNode);
}

} // namespace Tagging
} // namespace NSTU