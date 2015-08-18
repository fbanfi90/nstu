#include "Tagging/Location.h"

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "Windows/WindowsLocation.h"

namespace NSTU {
namespace Tagging {

// Default constructor.
Location::Location() : city(""), region(""), country(""), latitude(0.), longitude(0.), altitude(0.)
{
    // DEBUG: set Zurich to default location.
#ifdef NSTU_DEBUG
    city = "Zurich";
    region = "Zurich";
    country = "Switzerland";
    latitude = 47.366667;
    longitude = 8.55;
    altitude = 408.;
#endif
}

// Location constructor.
Location::Location(std::string city, std::string region, std::string country, double latitude, double longitude, double altitude) : city(city), region(region), country(country), latitude(latitude), longitude(longitude), altitude(altitude)
{
}

// The DateTime object representing current location.
Location Location::here()
{
    // Return location according to the system.
#if defined(WIN32) || defined(WIN64)
    return Windows::getWindowsLocation();
#else
    return Location();
#endif
}

// Write serialization.
void Location::write(cv::FileStorage& fileStorage) const
{
    fileStorage << "{";
    fileStorage << "City" << city;
    fileStorage << "Region" << region;
    fileStorage << "Country" << country;
    fileStorage << "Latitude" << latitude;
    fileStorage << "Longitude" << longitude;
    fileStorage << "Altitude" << altitude;
    fileStorage << "}";
}

// Read serialization.
void Location::read(const cv::FileNode& fileNode)
{
    city = (std::string)fileNode["City"];
    region = (std::string)fileNode["Region"];
    country = (std::string)fileNode["Country"];
    latitude = (double)fileNode["Latitude"];
    longitude = (double)fileNode["Longitude"];
    altitude = (double)fileNode["Altitude"];
}

// Out stream operator.
std::ostream& operator<<(std::ostream& stream, const Location& location)
{
    // Create string representation.
    stream << location.city << ", " << location.region << ", " << location.country;
    stream << " [" << location.latitude << ", " << location.longitude << ", " << location.altitude << "]";

    // Return the modified stream.
    return stream;
}

} // namespace Tagging
} // namespace NSTU