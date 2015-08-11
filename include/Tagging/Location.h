#ifndef LOCATION_H_
#define LOCATION_H_

// Standard C++.
#include <iostream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"
#include "ISerializable.h"

namespace NSTU
{
    class Location : ISerializable
    {
    public:

        // Default constructor.
        Location();

        // Location constructor.
        Location(std::string city, std::string region, std::string country, double latitude, double longitude, double altitude);

        // The Location object representing current location.
        static Location here();

        // Write serialization.
        void write(cv::FileStorage& fileStorage) const;

        // Read serialization.
        void read(const cv::FileNode& fileNode);

        // Out stream operator.
        friend std::ostream& operator<<(std::ostream& stream, const Location& location);

    public:

        // Location information.
        std::string city;
        std::string region;
        std::string country;
        double latitude;
        double longitude;
        double altitude;
    };

    // Must be defined for the serialization in FileStorage to work.
    static void write(cv::FileStorage& fs, const std::string&, const Location& location)
    {
        location.write(fs);
    }

    // Must be defined for the serialization in FileStorage to work.
    static void read(const cv::FileNode& fileNode, Location& location, const Location& defaultValue = Location())
    {
        if (fileNode.empty())
            location = defaultValue;
        else
            location.read(fileNode);
    }
}

#endif