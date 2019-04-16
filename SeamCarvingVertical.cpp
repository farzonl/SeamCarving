#include "SeamCarving.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cfloat> 

SeamCarvingVertical::SeamCarvingVertical(char* fileName, int seams) : 
    SeamCarving( cv::imread(fileName, cv::IMREAD_COLOR), seams)
{
    init();
}

cv::Mat SeamCarvingVertical::drawSeam(const cv::Mat &frame, const std::vector<int> &seam) {
    cv::Mat retMat = frame.clone();
    for(int col = 0; col < frame.cols; col++) {
        for(int row = 0; row < frame.rows; row++) {
            retMat.at<cv::Vec3b>(seam[col], col)[0] = 0;
            retMat.at<cv::Vec3b>(seam[col], col)[1] = 255;
            retMat.at<cv::Vec3b>(seam[col], col)[2] = 0;
        }
    }
    return retMat;
}

cv::Mat SeamCarvingVertical::computePathIntensityMat(const cv::Mat &rawEnergyMap) {
    cv::Mat pathIntensityMap = cv::Mat(rawEnergyMap.size(), CV_32FC1);

    if(rawEnergyMap.total() == 0 || pathIntensityMap.total() == 0) {
        return cv::Mat();
    }

    //First col of intensity paths is the same as the energy map
    rawEnergyMap.col(0).copyTo(pathIntensityMap.col(0));
    float max = 0;

    //The rest of them use the DP calculation using the minimum of the 3 pixels above them + their own intensity.
    for(int col = 1; col < pathIntensityMap.cols; col++) {
        for(int row = 0; row < pathIntensityMap.rows; row++) {
            //std::cout << "row: " << row << " col: " << col << std::endl;
            //The initial intensity of the pixel is its raw intensity
            float pixelIntensity = rawEnergyMap.at<float>(row, col);
            //The minimum intensity from the current path of the 3 pixels above it is added to its intensity.
            float p1 = intensity(pathIntensityMap.at<float>(row-1, col-1), row - 1, pathIntensityMap.rows);
            float p2 = intensity(pathIntensityMap.at<float>(row, col-1), row, pathIntensityMap.rows);
            float p3 = intensity(pathIntensityMap.at<float>(row+1, col-1), row + 1, pathIntensityMap.rows);

            float minIntensity = std::min(p1, p2);
            minIntensity = std::min(minIntensity, p3);

            pixelIntensity += minIntensity;

            max = std::max(max, pixelIntensity);
            pathIntensityMap.at<float>(row, col) = pixelIntensity;
        }
    }
	return pathIntensityMap;
}

std::vector<int> SeamCarvingVertical::getLeastImportantPath(const cv::Mat &importanceMap) {
    if(importanceMap.total() == 0)  {
        return std::vector<int>();
    }

    //Find the beginning of the least important path. Trying an averaging approach because absolute min wasn't very reliable.
    float minImportance = importanceMap.at<float>(0, importanceMap.cols - 1);
    int minRow = 0;
    for (int row = 1; row < importanceMap.rows; row++)
    {
        float currPixel =importanceMap.at<float>(row, importanceMap.cols - 1);
        if(currPixel < minImportance) {
            minRow = row;
            minImportance = currPixel;
        }
    }

    std::vector<int> leastEnergySeam(importanceMap.cols);
    leastEnergySeam[importanceMap.cols-1] = minRow;

    for(int col = importanceMap.cols - 2; col >= 0; col--) {
        float p1 = intensity(importanceMap.at<float>(minRow-1, col), minRow - 1, importanceMap.rows);
        float p2 = intensity(importanceMap.at<float>(minRow, col), minRow, importanceMap.rows);
        float p3 = intensity(importanceMap.at<float>(minRow+1, col), minRow + 1, importanceMap.rows);
        //Adjust the min column for path following
        if(p1 < p2 && p1 < p3) {
            minRow -= 1;
        } else if(p3 < p1 && p3 < p2) {
            minRow += 1;
        }
        leastEnergySeam[col] = minRow;
    }

    return leastEnergySeam;
}

cv::Mat SeamCarvingVertical::removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) {
    cv::Size orgSize = original.size();
    // new mat needs to shrink by one collumn
    cv::Size size = cv::Size(orgSize.width, orgSize.height-1);
    cv::Mat newMat = cv::Mat(size, original.type());

    unsigned char *rawOrig = original.data;
    unsigned char *rawOutput = newMat.data;
    for(int col = 0; col < seam.size(); col++) {
        removePixel(original, newMat, col, seam[col]);
    }
    return newMat;
}

void SeamCarvingVertical::removePixel(const cv::Mat &original, cv::Mat &outputMat, int minRow, int col) {
    cv::Mat origT = original.t();
    cv::Mat outT = outputMat.t();
    SeamCarving::removePixel(origT,outT, minRow, col);
    outputMat = outT.t();
}