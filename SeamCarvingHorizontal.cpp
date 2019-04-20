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

SeamCarvingHorizontal::SeamCarvingHorizontal(char* fileName, int seams, int grow) : 
    SeamCarving( cv::imread(fileName, cv::IMREAD_COLOR), seams, grow)
{
    this->sliderMax = this->image.cols;
    init();
}