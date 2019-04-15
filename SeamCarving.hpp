#ifndef __SEAM__CARVING_HPP__
#define __SEAM__CARVING_HPP__

#include <opencv2/core/core.hpp>

class SeamCarving
{
  public:
    SeamCarving(char* fileName, int seams=1);
    void showImage();

  private:
    cv::Mat GetEnergyImg(const cv::Mat &img);
    cv::Mat computeGradientMagnitude(const cv::Mat &frame);
    cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap);
    std::vector<int> getLeastImportantPath(const cv::Mat &importanceMap);
    cv::Mat removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam);
    cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam);
    //void removePixel(const cv::Mat &original, cv::Mat &importanceMap, int row, int minCol);
    float intensity(float currIndex, int col, int nCols);
    cv::Mat image;
    cv::Mat finalImage;
    int seams;
};

#endif // __SEAM__CARVING_HPP__
