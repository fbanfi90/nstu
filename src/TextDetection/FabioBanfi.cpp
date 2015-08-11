#include "TextDetection/FabioBanfi.h"

// Standard C++.
#include <iostream>
#include <limits>
#include <vector>

// OpenCV.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Boost.
#include <boost/unordered_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

// Project headers.
#include "TextDetection/ValuePoint.hpp"
#include "TextDetection/Ray.hpp"

namespace NSTU
{
    namespace TextDetection
    {
        namespace FabioBanfi
        {
            // Fabio Banfi implementation.
            std::vector<cv::Rect> getTextRegionsFB(const cv::Mat& image)
            {
                // Perform SWT for dark text on light background.
                auto swt1 = SWT(image, true, 1.f);
                cv::imshow("SWT1", swt1);

                // Perform SWT for light text on dark background.
                auto swt2 = SWT(image, false, 1.f);
                cv::imshow("SWT2", swt2);

                // Associate pixels to form connected components.
                auto components = associate(swt1);

                // Return the detected text regions.
                return std::vector<cv::Rect>();
            }

            // Stroke width transform (SWT).
            cv::Mat SWT(const cv::Mat& image, bool darkOnLight, float precision)
            {
                // Convert the original image to a new grayscale image.
                cv::Mat gray(image.size(), CV_8U);
                cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
                //cv::imshow("Gray", gray);

                // Detect images's edges.
                cv::Mat edges(image.size(), CV_8U);
                cv::Canny(gray, edges, 150., 400.); //175., 320.
                //cv::imshow("Edges", edges);//

                // Smooth the grayscale image.
                cv::Mat smoothed = cv::Mat(image.size(), CV_32F);
                gray.convertTo(smoothed, CV_32F, 1. / 255.);
                cv::GaussianBlur(smoothed, smoothed, cv::Size(5, 5), 0);
                //cv::imshow("Smoothed", smoothed);

                // Create Y and Y gradients.
                cv::Mat gradientX(image.size(), CV_32F, cv::Scalar(1.));
                cv::Mat gradientY(image.size(), CV_32F, cv::Scalar(1.));
                cv::Sobel(smoothed, gradientX, CV_32F, 1, 0);
                cv::Sobel(smoothed, gradientY, CV_32F, 0, 1);
                //cv::Scharr(smoothed, gradientX, CV_32F, 1, 0);
                //cv::Scharr(smoothed, gradientY, CV_32F, 0, 1);
                //cv::imshow("Gradient X", gradientX);
                //cv::imshow("Gradient Y", gradientY);

                // Initialize SWT output image, each pixel set to infinity by default.
                cv::Mat swt(image.size(), CV_32F, cv::Scalar(-1.));

                // Collection of detected rays.
                std::vector<Ray> rays;

                // Loop across each row.
                for (int row = 0; row < image.rows; ++row)
                {
                    // Loop across each column.
                    for (int col = 0; col < image.cols; ++col)
                    {
                        // Check wheter the pixel is an edge.
                        if (edges.at<uchar>(row, col) == 0xFFu)
                        {
                            // The ray to be computed for this pixel.
                            Ray ray;

                            // Set the first point of the ray.
                            ValuePoint p(col, row);
                            ray.p = p;
                            ray.points.push_back(p);

                            // The coordinates of the current pixel on the ray.
                            int currPixelX = col;
                            int currPixelY = row;

                            // The coordinates of the current real point on the ray.
                            float currValueX = (float)col + .5f;
                            float currValueY = (float)row + .5f;

                            // Get x and y components of the gradient.
                            float dx = gradientX.at<float>(row, col);
                            float dy = gradientY.at<float>(row, col);

                            // Get gradient magnitude.
                            float mag = sqrt(pow(dx, 2) + pow(dy, 2));

                            // Normalize gradient components according to the darkOnLight setting.
                            dx = (darkOnLight ? -dx : dx) / mag;
                            dy = (darkOnLight ? -dy : dy) / mag;

                            // Loop until another edge is found.
                            while (true)
                            {
                                // Advance in gradient direction by a small amount.
                                currValueX += dx * precision;
                                currValueY += dy * precision;

                                // Proceed only if we moved enough from the last pixel on the ray.
                                if ((int)(floor(currValueX)) != currPixelX || (int)(floor(currValueY)) != currPixelY)
                                {
                                    // Convert the current point to the closest pixel.
                                    currPixelX = (int)(floor(currValueX));
                                    currPixelY = (int)(floor(currValueY));

                                    // Terminate if the pixel is outside of the image's boundary.
                                    if (currPixelX < 0 || (currPixelX >= swt.cols) || currPixelY < 0 || (currPixelY >= swt.rows))
                                        break;

                                    // Insert the new point in the ray.
                                    ValuePoint pNew(currPixelX, currPixelY);
                                    ray.points.push_back(pNew);

                                    // Check whether last pixel on gradient direction was an edge.
                                    if (edges.at<uchar>(currPixelY, currPixelX) > 0)
                                    {
                                        // Set the last point of the ray.
                                        ray.q = pNew;

                                        // Get gradient of last point in the ray.
                                        float dxNew = gradientX.at<float>(currPixelY, currPixelX);
                                        float dyNew = gradientY.at<float>(currPixelY, currPixelX);
                                        mag = sqrt(pow(dxNew, 2) + pow(dyNew, 2));

                                        // Normalize gradient components according to the darkOnLight setting.
                                        dx = (darkOnLight ? -dx : dx) / mag;
                                        dy = (darkOnLight ? -dy : dy) / mag;

                                        // Add ray only if its angle with new point's gradient is less than 90�.
                                        if (acos(dx * -dxNew + dy * -dyNew) < CV_PI / 2.0)
                                        {
                                            // Calculate length of ray.
                                            float length = sqrt(pow((float)ray.q.x - (float)ray.p.x, 2) + pow((float)ray.q.y - (float)ray.p.y, 2));

                                            // Set the value of each pixel covered bay the ray to its length.
                                            for (auto& point : ray.points)
                                                swt.at<float>(point.y, point.x) = swt.at<float>(point.y, point.x) < 0 ? length : std::min(length, swt.at<float>(point.y, point.x));

                                            // Add this ray to the collection.
                                            rays.push_back(ray);
                                        }

                                        // Exit loop for this pixel.
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                /* Median filter */

                // Loop across all the found rays.
                for (auto& ray : rays)
                {
                    // Loop across all points of the ray and set their values.
                    for (auto& point : ray.points)
                        point.value = swt.at<float>(point.y, point.x);

                    // Sort the points by value.
                    std::sort(ray.points.begin(), ray.points.end());

                    // Find median value in the sorted list of points.
                    float median = (ray.points[ray.points.size() / 2]).value;

                    // Set new value in the image for each point in this ray.
                    for (auto& point : ray.points)
                        swt.at<float>(point.y, point.x) = std::min(point.value, median);
                }

                /* Normalization */

                // Get the maximal and minimal values in the SWT image.
                float maxVal = 0;
                float minVal = std::numeric_limits<float>::infinity();
                for (int row = 0; row < image.rows; ++row)
                {
                    for (int col = 0; col < image.cols; ++col)
                    {
                        if (swt.at<float>(row, col) >= 0.f)
                        {
                            maxVal = std::max(swt.at<float>(row, col), maxVal);
                            minVal = std::min(swt.at<float>(row, col), minVal);
                        }
                    }
                }

                // Get the difference between maximal and minimal values.
                float difference = maxVal - minVal;

                // Normalize each pixel.
                for (int row = 0; row < image.rows; ++row)
                    for (int col = 0; col < image.cols; ++col)
                        if (swt.at<float>(row, col) < 0)
                            swt.at<float>(row, col) = 1;
                        else
                            swt.at<float>(row, col) = (swt.at<float>(row, col) - minVal) / difference;

                // Convert SWT image from 32 bit float to 8 bit grayscale.
                swt.convertTo(swt, CV_8U, 255.);

                return swt;
            }

            // Associate pixels to form connected components.
            std::vector<std::vector<ValuePoint>> associate(const cv::Mat& swt)
            {
                boost::unordered_map<int, int> map;
                boost::unordered_map<int, ValuePoint> revmap;

                typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
                int verticesCount = 0;

                // Number vertices for graph.  Associate each point with number
                for (int row = 0; row < swt.rows; ++row)
                {
                    for (int col = 0; col < swt.cols; ++col)
                    {
                        if (swt.at<float>(row, col) > 0)
                        {
                            map[row * swt.cols + col] = verticesCount;
                            ValuePoint p;
                            p.x = col;
                            p.y = row;
                            revmap[verticesCount] = p;
                            verticesCount++;
                        }
                    }
                }

                Graph g(verticesCount);

                // Traverse and add edges to graph
                for (int row = 0; row < swt.rows; ++row)
                {
                    for (int col = 0; col < swt.cols; ++col)
                    {
                        if (swt.at<float>(row, col) > 0)
                        {
                            // Get the value of this pixel.
                            float value = swt.at<float>(row, col);

                            // check pixel to the right, right-down, down, left-down
                            int thisPixel = map[row * swt.cols + col];

                            if (col < swt.cols - 1)
                            {
                                float rightValue = swt.at<float>(row, col + 1);
                                if (rightValue > 0 && (value / rightValue <= 3.0 || rightValue / value <= 3.0))
                                    boost::add_edge(thisPixel, map.at(row * swt.cols + col + 1), g);
                            }

                            if (row < swt.rows - 1)
                            {
                                if (col < swt.cols - 1)
                                {
                                    float rightDownValue = swt.at<float>(row + 1, col + 1);
                                    if (rightDownValue > 0 && (value / rightDownValue <= 3.0 || rightDownValue / value <= 3.0))
                                        boost::add_edge(thisPixel, map.at((row+1) * swt.cols + col + 1), g);
                                }

                                float downValue = swt.at<float>(row + 1, col);
                                if (downValue > 0 && (value / downValue <= 3.0 || downValue / value <= 3.0))
                                    boost::add_edge(thisPixel, map.at((row+1) * swt.cols + col), g);

                                if (col > 0)
                                {
                                    float leftDownValue = downValue = swt.at<float>(row + 1, col - 1);
                                    if (leftDownValue > 0 && (value / leftDownValue <= 3.0 || leftDownValue / value <= 3.0))
                                        boost::add_edge(thisPixel, map.at((row+1) * swt.cols + col - 1), g);
                                }
                            }
                        }
                    }
                }

                std::vector<int> c(verticesCount);

                int componentsCount = connected_components(g, &c[0]);

                std::vector<std::vector<ValuePoint> > components;
                components.reserve(componentsCount);
                //std::cout << "components:\t " << componentsCount << "\tvertices:\t" << verticesCount << std::endl;
                for (int j = 0; j < componentsCount; j++) {
                    std::vector<ValuePoint> tmp;
                    components.push_back( tmp );
                }
                for (int j = 0; j < verticesCount; j++) {
                    ValuePoint p = revmap[j];
                    (components[c[j]]).push_back(p);
                }

                return components;
            }
        }
    }
}