#ifndef __SEAM__CARVING_HPP__
#define __SEAM__CARVING_HPP__

#include <opencv2/core/core.hpp>
#define DEBUG 1

class SeamCarving {
  public:
    void showImage();
    const cv::Mat& getFinalImage();
    virtual void computeNewFinalImage(int pos);
    void setBlockUpdate(bool bUpdate);
    bool getBlockUpdateStatus();
    void showSeamsImg();
  protected:
    SeamCarving(const cv::Mat &img,int seams, int grow);
    void init();
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) = 0;
    cv::Mat image;
    cv::Mat finalImage;
    int seams;
    int grow;
    int sliderMax;
    int sliderPos;
    std::vector<std::vector<int>> vecSeams;

  private:
    cv::Mat GetEnergyImg(const cv::Mat &img);
    cv::Mat computeGradientMagnitude(const cv::Mat &frame);
    float intensity(float currIndex, int start, int end);
    cv::Mat computePathIntensityMat(const cv::Mat &rawEnergyMap);
    std::vector<int> getLeastImportantPath(const cv::Mat &importanceMap);
    cv::Mat removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam);
    void removePixel(const cv::Mat &original, cv::Mat &outputMap, int row, int minCol);
    cv::Mat addLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam);
    void addPixel(const cv::Mat &original, cv::Mat &outputMat, int row, int minCol);
    bool blockUpdate = false;

};

class SeamCarvingHorizontal : public SeamCarving
{
  public:
    SeamCarvingHorizontal(char* fileName, int seams=100, int grow=0);
  protected:
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) override;
};

class SeamCarvingVertical : public SeamCarving {
  public:
    SeamCarvingVertical(char* fileName, int seams=100, int grow=0);
    virtual void computeNewFinalImage(int pos) override;
  protected:
    virtual cv::Mat drawSeam(const cv::Mat &frame, const std::vector<int> &seam) override;
};

#endif // __SEAM__CARVING_HPP__
