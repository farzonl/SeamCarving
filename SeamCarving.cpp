#include "SeamCarving.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cfloat> 

SeamCarving::SeamCarving(const cv::Mat &img,int seams) : image(img), seams(seams) {}

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
#if DEBUG
        this->vecSeams.push_back(seam);
#endif
        newFrame = removeLeastImportantPath(newFrame,seam);
        if(newFrame.rows == 0 && newFrame.cols == 0) {
            this->finalImage = this->image;
            break;
        }
    }
    this->finalImage = newFrame;
}

void SeamCarving::showImage() {
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
    
    cv::imshow("gradient Image", u8_image);

    namedWindow( "intensity Image", cv::WINDOW_AUTOSIZE );
    cv::Mat u8_image2;
    cv::Mat intensityMat = computePathIntensityMat(gradient);
    cv::Mat dst;
    cv::normalize(intensityMat, dst, 0, 255, cv::NORM_MINMAX);
    dst.convertTo(u8_image2, CV_8U);
    cv::imshow( "intensity Image", u8_image2);

    //cv::Mat engImg = GetEnergyImg(image);
    //namedWindow("energy Image", cv::WINDOW_AUTOSIZE);
    //cv::Mat u8_image3;
    //engImg.convertTo(u8_image3, CV_8U);
    //cv::imshow( "energy Image", u8_image3);
    cv::Mat seamsFrame = image.clone();
    for(int i = 0; i < this->vecSeams.size(); i++) {
        seamsFrame = drawSeam(seamsFrame, this->vecSeams[i]);
    }
    namedWindow("Image Seams", cv::WINDOW_AUTOSIZE);
    cv::imshow( "Image Seams", seamsFrame);
#endif

    namedWindow( "Final Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Final Image", finalImage );
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