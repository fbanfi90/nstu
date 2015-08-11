#ifndef VALUE_POINT_H_
#define VALUE_POINT_H_

// Project headers.
#include "../settings.h"

namespace  NSTU
{
    namespace TextDetection
    {
        namespace FabioBanfi
        {
            struct ValuePoint
            {
                int x;
                int y;
                float value;

                // Default constructor.
                ValuePoint(int x = 0, int y = 0) : x(x), y(y), value(0)
                {
                }

                // Copy constructor.
                ValuePoint(const ValuePoint& vp) : x(vp.x), y(vp.y), value(vp.value)
                {
                }

                // Comparator.
                bool operator<(const ValuePoint& vp) const
                {
                    return value < vp.value;
                }
            };
        }
    }
}

#endif