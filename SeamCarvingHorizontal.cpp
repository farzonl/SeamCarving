#include "SeamCarving.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cfloat> 


cv::Mat SeamCarvingHorizontal::drawSeam(const cv::Mat &frame, const std::vector<int> &seam) {
    cv::Mat retMat = frame.clone();
    for(int row = 0; row < frame.rows; row++) {
        for(int col = 0; col < frame.cols; col++) {
            retMat.at<cv::Vec3b>(row, seam[row])[0] = 0;
            retMat.at<cv::Vec3b>(row, seam[row])[1] = 255;
            retMat.at<cv::Vec3b>(row, seam[row])[2] = 0;
        }
    }
    return retMat;
}

SeamCarvingHorizontal::SeamCarvingHorizontal(char* fileName, int seams) : 
    SeamCarving( cv::imread(fileName, cv::IMREAD_COLOR), seams)
{
    init();
}


cv::Mat SeamCarvingHorizontal::computePathIntensityMat(const cv::Mat &rawEnergyMap) {
    cv::Mat pathIntensityMap = cv::Mat(rawEnergyMap.size(), CV_32FC1);

    if(rawEnergyMap.total() == 0 || pathIntensityMap.total() == 0) {
        return cv::Mat();
    }

    //First row of intensity paths is the same as the energy map
    rawEnergyMap.row(0).copyTo(pathIntensityMap.row(0));
    float max = 0;

    //The rest of them use the DP calculation using the minimum of the 3 pixels above them + their own intensity.
    for(int row = 1; row < pathIntensityMap.rows; row++) {
        for(int col = 0; col < pathIntensityMap.cols; col++) {
            //The initial intensity of the pixel is its raw intensity
            float pixelIntensity = rawEnergyMap.at<float>(row, col);
            //The minimum intensity from the current path of the 3 pixels above it is added to its intensity.
            float p1 = intensity(pathIntensityMap.at<float>(row-1, col-1), col - 1, pathIntensityMap.cols);
            float p2 = intensity(pathIntensityMap.at<float>(row-1, col), col, pathIntensityMap.cols);
            float p3 = intensity(pathIntensityMap.at<float>(row-1, col+1), col + 1, pathIntensityMap.cols);

            float minIntensity = std::min(p1, p2);
            minIntensity = std::min(minIntensity, p3);

            pixelIntensity += minIntensity;

            max = std::max(max, pixelIntensity);
            pathIntensityMap.at<float>(row, col) = pixelIntensity;
        }
    }
	return pathIntensityMap;
}

std::vector<int> SeamCarvingHorizontal::getLeastImportantPath(const cv::Mat &importanceMap) {
    if(importanceMap.total() == 0)  {
        return std::vector<int>();
    }

    //Find the beginning of the least important path. Trying an averaging approach because absolute min wasn't very reliable.
    float minImportance = importanceMap.at<float>(importanceMap.rows - 1, 0);
    int minCol = 0;
    for (int col = 1; col < importanceMap.cols; col++)
    {
        float currPixel =importanceMap.at<float>(importanceMap.rows - 1, col);
        if(currPixel < minImportance) {
            minCol = col;
            minImportance = currPixel;
        }
    }

    std::vector<int> leastEnergySeam(importanceMap.rows);
    leastEnergySeam[importanceMap.rows-1] = minCol;
    for(int row = importanceMap.rows - 2; row >= 0; row--) {
        float p1 = intensity(importanceMap.at<float>(row, minCol-1), minCol - 1, importanceMap.cols);
        float p2 = intensity(importanceMap.at<float>(row, minCol), minCol, importanceMap.cols);
        float p3 = intensity(importanceMap.at<float>(row, minCol+1), minCol + 1, importanceMap.cols);
        //Adjust the min column for path following
        if(p1 < p2 && p1 < p3) {
            minCol -= 1;
        } else if(p3 < p1 && p3 < p2) {
            minCol += 1;
        }
        leastEnergySeam[row] = minCol;
    }

    return leastEnergySeam;
}

cv::Mat SeamCarvingHorizontal::removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) {
    cv::Size orgSize = original.size();
    // new mat needs to shrink by one collumn
    cv::Size size = cv::Size(orgSize.width-1, orgSize.height);
    cv::Mat newMat = cv::Mat(size, original.type());

    unsigned char *rawOrig = original.data;
    unsigned char *rawOutput = newMat.data;
    for(int row = 0; row < seam.size(); row++) {
        removePixel(original, newMat, row, seam[row]);
    }
    return newMat;
}