#include "SeamCarving.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#ifndef __clang__
#include <filesystem>
#endif
#include <cfloat> 

SeamCarving::SeamCarving(const cv::Mat &img, int seams, bool grow) : 
image(img), seams(seams), grow(grow) {}

void SeamCarving::init() {
    cv::Mat newFrame = image.clone();

    for(int i = 0; i < seams; i++) {
        //Gradient Magnitude for intensity of image.
        cv::Mat gradientMagnitude = computeGradientMagnitude(newFrame);
        //Use DP to create the real energy map that is used for path calculation.  
        // Strictly using vertical paths for testing simplicity.
        cv::Mat pathIntensityMat = computePathIntensityMat(gradientMagnitude);

        if(pathIntensityMat.rows == 0 && pathIntensityMat.cols == 0) {
            this->finalImage = this->image;
            break;
        }
        std::vector<int> seam = getLeastImportantPath(pathIntensityMat);
        this->vecSeams.push_back(seam);
        
        newFrame = removeLeastImportantPath(newFrame,seam);
        
        if(newFrame.rows == 0 && newFrame.cols == 0) {
            this->finalImage = this->image;
            break;
        }
    }
    
    if (grow) {
        cv::Mat growMat = image.clone();

        for (int i = 0; i < this->vecSeams.size(); i++) {
            growMat = addLeastImportantPath(growMat,this->vecSeams[i]);
        }
        this->finalImage = growMat;
    } else {
        this->finalImage = newFrame;
    }

    this->sliderPos = seams;
    
}

void SeamCarving::computeNewFinalImage(int sliderPos) {
    if(sliderPos == 0) {
        this->finalImage =  this->image;
        return;
    }
    if(sliderPos < 1 || sliderPos >= sliderMax-1) {
        return;
    }
    if(sliderPos > vecSeams.size()) {
        cv::Mat newFrame = finalImage.clone();
        for(int i = vecSeams.size()-1; i < this->sliderPos; i++) {
            //Gradient Magnitude for intensity of image.
            cv::Mat gradientMagnitude = computeGradientMagnitude(newFrame);
            //Use DP to create the real energy map that is used for path calculation.  
            // Strictly using vertical paths for testing simplicity.
            cv::Mat pathIntensityMat = computePathIntensityMat(gradientMagnitude);

            if(pathIntensityMat.rows == 0 && pathIntensityMat.cols == 0) {
                this->finalImage = this->image;
                break;
            }
            std::vector<int> seam = getLeastImportantPath(pathIntensityMat);
            this->vecSeams.push_back(seam);
            newFrame = removeLeastImportantPath(newFrame,seam);
            if(newFrame.rows == 0 && newFrame.cols == 0) {
                this->finalImage = this->image;
                break;
            }
        }
        this->finalImage = newFrame;
    } else if (sliderPos < vecSeams.size()) {
        cv::Mat newFrame = image.clone();
        for(int i = 0; i < this->sliderPos; i++) { // TODO check if it is faster to add seams back (probably not)
            newFrame = removeLeastImportantPath(newFrame,vecSeams[i]);
            if(newFrame.rows == 0 && newFrame.cols == 0) {
                this->finalImage = this->image;
                break;
            }
        }
        this->finalImage = newFrame;
    }
}

const cv::Mat& SeamCarving::getFinalImage() {
    return this->finalImage;
}

void SeamCarving::showSeamsImg() {
    cv::Mat seamsFrame = image.clone();
    //std::cout << "sliderPos: " << sliderPos << std::endl;
    for(int i = 0; i < sliderPos; i++) {
        seamsFrame = drawSeam(seamsFrame, this->vecSeams[i]);
    }
    cv::imwrite("output/seams_image.jpg", seamsFrame);
    cv::imshow( "Image Seams", seamsFrame);
}

static void onChange( int pos, void* object )
{
   SeamCarving* sc = (SeamCarving*)(object);
   /*if(sc->getBlockUpdateStatus()) {
       return;
   }*/
   sc->computeNewFinalImage(pos);
   imshow("Final Image", sc->getFinalImage());
#if DEBUG
    sc->showSeamsImg();
#endif
}
static void onMouse( int event, int x, int y, int, void* object)
{
    SeamCarving* sc = (SeamCarving*)(object);
    if( event == cv::EVENT_LBUTTONDOWN || 
        event == cv::EVENT_RBUTTONDOWN ||
        event == cv::EVENT_MBUTTONDOWN
      ) {
          sc->setBlockUpdate(true);
    } else if(cv::EVENT_LBUTTONUP ||
              cv::EVENT_RBUTTONUP ||
              cv::EVENT_MBUTTONUP) { 
         sc->setBlockUpdate(false);
     }
}

void SeamCarving::setBlockUpdate(bool bUpdate) {
    this->blockUpdate = bUpdate;
}

 bool SeamCarving::getBlockUpdateStatus() {
     return this->blockUpdate;
 }

void SeamCarving::showImage() {
#ifndef __clang__
    if(!std::filesystem::exists("output")) {
        std::filesystem::create_directory("output");
    }
#endif
    if( image.empty() ) 
    {
        std::cout <<  "Could not open raw image" << std::endl ;
        return;
    }
    namedWindow( "Raw Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Raw Image", image );

    if( finalImage.empty() ) 
    {
        std::cout <<  "Could not open final image" << std::endl ;
        return;
    }
#if DEBUG
    namedWindow( "gradient Image", cv::WINDOW_AUTOSIZE );
    cv::Mat gradient = computeGradientMagnitude(image);
    cv::Mat u8_image;
    gradient.convertTo(u8_image, CV_8U);
    
    cv::imwrite("output/gradient_image.jpg", u8_image);
    cv::imshow("gradient Image", u8_image);

    namedWindow( "intensity Image", cv::WINDOW_AUTOSIZE );
    cv::Mat u8_image2;
    cv::Mat intensityMat = computePathIntensityMat(gradient);
    cv::Mat dst;
    cv::normalize(intensityMat, dst, 0, 255, cv::NORM_MINMAX);
    dst.convertTo(u8_image2, CV_8U);
    cv::imwrite("output/intensity_image.jpg", u8_image2);
    cv::imshow( "intensity Image", u8_image2);

    //cv::Mat engImg = GetEnergyImg(image);
    //namedWindow("energy Image", cv::WINDOW_AUTOSIZE);
    //cv::Mat u8_image3;
    //engImg.convertTo(u8_image3, CV_8U);
    //cv::imshow( "energy Image", u8_image3);
    namedWindow("Image Seams", cv::WINDOW_AUTOSIZE);
    showSeamsImg();
    
#endif

    namedWindow( "Final Image", cv::WINDOW_AUTOSIZE );
    cv::createTrackbar("Seams", "Final Image", &sliderPos, sliderMax, onChange, this);
    //cv::setMouseCallback("Final Image", onMouse, this );
    cv::imwrite("output/final_image.jpg", finalImage);
    cv::imshow("Final Image", finalImage);
    cv::waitKey(0); 
}

cv::Mat SeamCarving::GetEnergyImg(const cv::Mat &img) {
      // find partial derivative of x-axis and y-axis seperately
      // sum up the partial derivates
      float pd[] = {1, 2, 1, 0, 0, 0, -1, -2 - 1};
      cv::Mat xFilter(3, 3, CV_32FC1, pd);
      cv::Mat yFilter = xFilter.t();
      cv::Mat grayImg;
      cv::cvtColor(img, grayImg, cv::COLOR_RGBA2GRAY);
      cv::Mat dxImg;
      cv::Mat dyImg;

      cv::filter2D(grayImg, dxImg, 0, xFilter);
      cv::filter2D(grayImg, dyImg, 0, yFilter);
      //cv::Mat zeroMat = cv::Mat::zeros(dxImg.rows, dxImg.cols, dxImg.type());
      //cv::Mat absDxImg;
      //cv::Mat absDyImg;
      //cv::absdiff(dxImg, zeroMat, absDxImg);
      //cv::absdiff(dyImg, zeroMat, absDyImg);
      cv::Mat absDxImg = cv::abs(dxImg);
      cv::Mat absDyImg = cv::abs(dyImg);

      cv::Mat energyImg;
      cv::add(absDxImg, absDyImg, energyImg);
      return energyImg;
  }

cv::Mat SeamCarving::computeGradientMagnitude(const cv::Mat &frame) {
    cv::Mat grayScale;
    cv::cvtColor(frame, grayScale, cv::COLOR_RGBA2GRAY);
    cv::Mat drv = cv::Mat(grayScale.size(), CV_16SC1);
    cv::Mat drv32f = cv::Mat(grayScale.size(), CV_32FC1);
    cv::Mat mag = cv::Mat::zeros(grayScale.size(), CV_32FC1);
    Sobel(grayScale, drv, CV_16SC1, 1, 0);
    drv.convertTo(drv32f, CV_32FC1);
    cv::accumulateSquare(drv32f, mag);
    Sobel(grayScale, drv, CV_16SC1, 0, 1);
    drv.convertTo(drv32f, CV_32FC1);
    cv::accumulateSquare(drv32f, mag);
    cv::sqrt(mag, mag);
    return mag;
}

float SeamCarving::intensity(float currIndex, int start, int end) {
    if(start < 0 || start >= end) {
    	return FLT_MAX;
    } else {
    	return currIndex;
    }
}

cv::Mat SeamCarving::computePathIntensityMat(const cv::Mat &rawEnergyMap) {
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

std::vector<int> SeamCarving::getLeastImportantPath(const cv::Mat &importanceMap) {
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

cv::Mat SeamCarving::removeLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) {
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

void SeamCarving::removePixel(const cv::Mat &original, cv::Mat &outputMat, int row, int minCol) {
    int width = original.cols;
    int channels = original.channels();
    int originRowStart = row * channels * width;
    int newRowStart = row * channels * (width - 1);
    int firstNum = minCol * channels;
    unsigned char *rawOrig = original.data;
    unsigned char *rawOutput = outputMat.data;

    //std::cout << "originRowStart: " << originRowStart << std::endl;
    //std::cout << "newRowStart: " << newRowStart << std::endl;
    //std::cout << "firstNum: " << firstNum << std::endl;
    memcpy(rawOutput + newRowStart, rawOrig + originRowStart, firstNum);

    int originRowMid = originRowStart + (minCol + 1) * channels;
    int newRowMid = newRowStart + minCol * channels;
    int secondNum = (width - 1) * channels - firstNum;

    //std::cout << "originRowMid: " << originRowMid << std::endl;
    //std::cout << "newRowMid: " << newRowMid << std::endl;
    //std::cout << "secondNum: " << secondNum << std::endl;
    memcpy(rawOutput + newRowMid, rawOrig + originRowMid, secondNum);

    int leftPixel = minCol - 1;
    int rightPixel = minCol + 1;

    int byte1 = rawOrig[originRowStart + minCol * channels];
    int byte2 = rawOrig[originRowStart + minCol * channels + 1];
    int byte3 = rawOrig[originRowStart + minCol * channels + 2];

    if (byte1 < 0)
        byte1 += 256;
    if (byte2 < 0)
        byte2 += 256;
    if (byte3 < 0)
        byte3 += 256;

    if (rightPixel < width) {
        int byte1R = rawOrig[originRowStart + rightPixel * channels];
        int byte2R = rawOrig[originRowStart + rightPixel * channels + 1];
        int byte3R = rawOrig[originRowStart + rightPixel * channels + 2];
        if (byte1R < 0)
            byte1R += 256;
        if (byte2R < 0)
            byte2R += 256;
        if (byte3R < 0)
            byte3R += 256;
        rawOutput[newRowStart + minCol * channels] = (unsigned char)((byte1 + byte1R) / 2);
        rawOutput[newRowStart + minCol * channels + 1] = (unsigned char)((byte2 + byte2R) / 2);
        rawOutput[newRowStart + minCol * channels + 2] = (unsigned char)((byte3 + byte3R) / 2);
    }

    if(leftPixel >= 0) {
        int byte1L = rawOrig[originRowStart + leftPixel*channels];
        int byte2L = rawOrig[originRowStart + leftPixel*channels+1];
        int byte3L = rawOrig[originRowStart + leftPixel*channels+2];
        if(byte1L < 0) byte1L += 256;
        if(byte2L < 0) byte2L += 256;
        if(byte3L < 0) byte3L += 256;
        rawOutput[newRowStart + leftPixel*channels] = (unsigned char) ((byte1 + byte1L)/2);
        rawOutput[newRowStart + leftPixel*channels+1] = (unsigned char) ((byte2 + byte2L)/2);
        rawOutput[newRowStart + leftPixel*channels+2] = (unsigned char) ((byte3 + byte3L)/2);
    }
}

cv::Mat SeamCarving::addLeastImportantPath(const cv::Mat &original, const std::vector<int> &seam) {
    cv::Size orgSize = original.size();
    // new mat needs to grow by one column
    cv::Size size = cv::Size(orgSize.width+1, orgSize.height);
    cv::Mat newMat = cv::Mat(size, original.type());

    unsigned char *rawOrig = original.data;
    unsigned char *rawOutput = newMat.data;
    for(int row = 0; row < seam.size(); row++) {
        //std::cout << "row: " << row << ", col: " << seam[row] << std::endl;
        addPixel(original, newMat, row, seam[row]);
    }
    return newMat;
}

void SeamCarving::addPixel(const cv::Mat &original, cv::Mat &outputMat, int row, int minCol) {
    int width = original.cols;
    int channels = original.channels();
    int originRowStart = row * channels * width;
    int newRowStart = row * channels * (width + 1);
    int firstNum = (minCol + 1) * channels;

    unsigned char *rawOrig = original.data;
    unsigned char *rawOutput = outputMat.data;

    memcpy(rawOutput + newRowStart, rawOrig + originRowStart, firstNum);

    memcpy(rawOutput + newRowStart + firstNum, rawOrig + originRowStart + firstNum, channels);

    int originRowMid = originRowStart + ((minCol + 1) * channels);
    int newRowMid = newRowStart + ((minCol + 2) * channels);
    int secondNum = (width * channels) - firstNum;

    memcpy(rawOutput + newRowMid, rawOrig + originRowMid, secondNum);

    int leftPixel = minCol - 1;
    int rightPixel = minCol + 1;

    int byte1 = rawOrig[originRowStart + minCol * channels];
    int byte2 = rawOrig[originRowStart + minCol * channels + 1];
    int byte3 = rawOrig[originRowStart + minCol * channels + 2];

    if (byte1 < 0)
        byte1 += 256;
    if (byte2 < 0)
        byte2 += 256;
    if (byte3 < 0)
        byte3 += 256;

    if (rightPixel < width) {
        int byte1R = rawOrig[originRowStart + rightPixel * channels];
        int byte2R = rawOrig[originRowStart + rightPixel * channels + 1];
        int byte3R = rawOrig[originRowStart + rightPixel * channels + 2];
        if (byte1R < 0)
            byte1R += 256;
        if (byte2R < 0)
            byte2R += 256;
        if (byte3R < 0)
            byte3R += 256;
        rawOutput[newRowStart + minCol * channels] = (unsigned char)((byte1 + byte1R) / 2);
        rawOutput[newRowStart + minCol * channels + 1] = (unsigned char)((byte2 + byte2R) / 2);
        rawOutput[newRowStart + minCol * channels + 2] = (unsigned char)((byte3 + byte3R) / 2);
    }

    if(leftPixel >= 0) {
        int byte1L = rawOrig[originRowStart + leftPixel*channels];
        int byte2L = rawOrig[originRowStart + leftPixel*channels+1];
        int byte3L = rawOrig[originRowStart + leftPixel*channels+2];
        if(byte1L < 0) byte1L += 256;
        if(byte2L < 0) byte2L += 256;
        if(byte3L < 0) byte3L += 256;
        rawOutput[newRowStart + leftPixel*channels] = (unsigned char) ((byte1 + byte1L)/2);
        rawOutput[newRowStart + leftPixel*channels+1] = (unsigned char) ((byte2 + byte2L)/2);
        rawOutput[newRowStart + leftPixel*channels+2] = (unsigned char) ((byte3 + byte3L)/2);
    }
}