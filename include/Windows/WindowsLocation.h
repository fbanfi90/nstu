#pragma once

#if defined(WIN32) || defined(WIN64)

// Windows.
#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <LocationApi.h>

// Project headers.
#include "../Tagging/Location.h"

namespace NSTU {
namespace Windows {

Location getWindowsLocation()
{
    // The location to be returned.
    Location location;

    // Initialize COM.
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        // Scope so that smart pointers CComPtr are destructed before CoUninit is called.
        {
            // Main Location interface.
            CComPtr<ILocation> spLocation;

            // Create the Location object.
            if (SUCCEEDED(spLocation.CoCreateInstance(CLSID_Location)))
            {
                // Array of report types of interest. Other ones include IID_ICivicAddressReport
                IID REPORT_TYPES[] = { IID_ILatLongReport };

                // If request failed return default location.
                if (!FAILED(spLocation->RequestPermissions(NULL, REPORT_TYPES, ARRAYSIZE(REPORT_TYPES), TRUE)))
                {
                    // Location report object.
                    CComPtr<ILocationReport> spLocationReport;

                    // LatLong report object.
                    CComPtr<ILatLongReport> spLatLongReport;

                    // Get the current location report, then get the ILatLongReport interface from ILocationReport and ensure it isn't NULL.
                    if ((SUCCEEDED(spLocation->GetReport(IID_ILatLongReport, &spLocationReport))) && (SUCCEEDED(spLocationReport->QueryInterface(&spLatLongReport))))
                    {
                        // Retrive location information.
                        double latitude, longitude, altitude;
                        latitude = SUCCEEDED(spLatLongReport->GetLatitude(&latitude)) ? latitude : 0.;
                        longitude = SUCCEEDED(spLatLongReport->GetLongitude(&longitude)) ? longitude : 0.;
                        altitude = SUCCEEDED(spLatLongReport->GetAltitude(&altitude)) ? altitude : 0.;

                        // Return retrived location.
                        location = Location("", "", "", latitude, longitude, altitude);
                    }
                }
            }
        }

        // Release COM (sometimes makes the program crash - not really necessary).
        //CoUninitialize();
    }

    // Return the location.
    return location;
}

} // Windows
} // NSTU

#endif