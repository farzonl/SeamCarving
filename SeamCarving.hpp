#ifndef __SEAM__CARVING_HPP__
#define __SEAM__CARVING_HPP__

#include <opencv2/core/core.hpp>

class SeamCarving
{
  public:
    SeamCarving(char* fileName, int seams=300);
    void showImage();

  private:
    cv::Mat GetEnergyImg(const cv::Mat &img);
    cv::Mat computeGradientMagnitude(const cv::Mat &frame);
    cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap);
    cv::Mat removeLeastImportantPath(const cv::Mat &original, const cv::Mat &importanceMap);
    void removePixel(const cv::Mat &original, cv::Mat &importanceMap, int row, int minCol);
    float intensity(float currIndex, int col, int nCols);
    cv::Mat image;
    cv::Mat finalImage;
    int seams;
};

#endif // __SEAM__CARVING_HPP__
