#ifndef __SEAM__CARVING_HPP__
#define __SEAM__CARVING_HPP__

#include <opencv2/core/core.hpp>
#define DEBUG 1

class SeamCarving {
  public:
    void showImage();
  protected:
    SeamCarving(const cv::Mat &img,int seams);
    void init();
    cv::Mat GetEnergyImg(const cv::Mat &img);
    cv::Mat computeGradientMagnitude(const cv::Mat &frame);
    float intensity(float currIndex, int start, int end);

    virtual cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap) = 0;
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) = 0;
    virtual std::vector<int> getLeastImportantPath(const cv::Mat &importanceMap) = 0;
    virtual cv::Mat removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) = 0;
    virtual void removePixel(const cv::Mat &original, cv::Mat &outputMap, int row, int minCol);
    cv::Mat image;
    cv::Mat finalImage;
    int seams;
#if DEBUG
     std::vector<std::vector<int>> vecSeams;
#endif

};

class SeamCarvingHorizontal : public SeamCarving
{
  public:
    SeamCarvingHorizontal(char* fileName, int seams=100);
  protected:
    virtual cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap) override;
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) override;
    virtual std::vector<int> getLeastImportantPath(const cv::Mat &importanceMap) override;
    virtual cv::Mat removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) override;
};

class SeamCarvingVertical : public SeamCarving {
  public:
    SeamCarvingVertical(char* fileName, int seams=100);
  protected:
    virtual cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap) override;
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) override;
    virtual std::vector<int> getLeastImportantPath(const cv::Mat &importanceMap) override;
    virtual cv::Mat removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) override;
    virtual void removePixel(const cv::Mat &original, cv::Mat &outputMap, int minRow, int col) override;
};

#endif // __SEAM__CARVING_HPP__
