#ifndef MY_ENHANCE

#define MY_ENHANCE



#include <iostream>  
#include <stdio.h>  


#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv/highgui.h"
#include "opencv/cv.hpp"

using namespace std;
using namespace cv;

void highlight(Mat &frame);
void MySkinEnhance(Mat &frame);
void SkinRGB(IplImage* src,IplImage* dst);
Mat equalizeChannelHist(const Mat & inputImage); 
Mat equalizeIntensityHist(const Mat & inputImage);
#endif