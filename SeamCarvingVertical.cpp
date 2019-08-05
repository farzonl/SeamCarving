#include "SeamCarving.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cfloat> 

SeamCarvingVertical::SeamCarvingVertical(char* fileName, int seams, bool grow) : 
    SeamCarving( cv::imread(fileName, cv::IMREAD_COLOR), seams, grow)
{
    this->sliderMax = this->image.rows;
    cv::Mat oldImage = this->image;
    this->image = this->image.t();
    init();
    this->image = oldImage;
    this->finalImage =this->finalImage.t();
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

void SeamCarvingVertical::computeNewFinalImage(int pos) {
    cv::Mat oldImage = this->image;
    this->image = this->image.t();
    SeamCarving::computeNewFinalImage(pos);
    this->image = oldImage;
    this->finalImage =this->finalImage.t();
}
#if DEBUG
void SeamCarvingVertical::showSeamsImg() {
    cv::Mat oldImage = this->image;
    this->image = this->image.t();
    SeamCarving::showImage();
    this->image = oldImage;
}
#endif