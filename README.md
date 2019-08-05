# Seam Carving

## Code status
[![codecov](https://codecov.io/gh/farzonl/SeamCarving/branch/master/graph/badge.svg)](https://codecov.io/gh/farzonl/SeamCarving)
[![Build Status](https://travis-ci.com/farzonl/SeamCarving.svg?branch=master)](https://travis-ci.com/farzonl/SeamCarving)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/farzonl/SeamCarving.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/farzonl/SeamCarving/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/farzonl/SeamCarving.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/farzonl/SeamCarving/context:cpp)

## Demo
[![Fluid Simulation](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/Result.png)](https://youtu.be/Curd1u6-itE)

## Dependencies
- opencv
### For Mac
```
brew install opencv
```
### For Linux
```
sudo apt-get install libopencv-dev pkg-config
```
### For Windows
- VS was never tested.
- As a work around you can install the linux subsystem for windows and Xmig for X11 windows
   run the linux steps.

## how to build?
```
make
```
## how to run?
- basic usage horizontal shrinking with fixed number of seams:
```
./exec <image>.png|jpg 
```
- vertical or horizontal shrinking with fixed number of seams:
```
./exec <image>.png -h|-v
```
- vertical or horizontal shrinking with variable number of seams:
```
./exec <image>.png -h|-v -n <number>
```
-Toggle shrink or grow usage.
```
./exec <image>.png -h|-v -r|-g -n <number>
```

## Code Description
![](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/codeDesc.png)

## High Level Pipeline
![](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/pipeline.png)

## Grow Pipeline
![](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/growingpipeline.png)

## Shrink Pipeline
![](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/shrinkingpipeline.png)

## Class Presentation
![presentation](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/presentation.pdf)

## Final Report
![report](https://raw.githubusercontent.com/farzonl/SeamCarving/master/documentation/report.pdf)