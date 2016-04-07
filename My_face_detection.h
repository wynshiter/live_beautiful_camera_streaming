#ifndef MY_FACE_DETETION_ENHANCE
#define  MY_FACE_DETETION_ENHANCE

#include <iostream>
#include <string>
#include <stdio.h>  


#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/objdetect/objdetect.hpp"
using namespace std;
using namespace cv;

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
extern string face_cascade_name;// = "haarcascade_frontalface_alt.xml";

extern CascadeClassifier face_cascade;
extern CascadeClassifier eyes_cascade;

extern string window_name_onlyface;// = "Capture - only Face";
extern string window_name_face;// = "Capture - Face ";
//extern RNG rng;

//extern const int FRAME_WIDTH ;//= 640;
//extern const int FRAME_HEIGHT ;//= 480;

/** Function Headers */
void detectAndenhance( Mat &frame );
///对于四个通道图像的操作
void mapToMat(const cv::Mat &srcAlpha, cv::Mat &dest, int x, int y);
void detectAndDisplay( Mat frame );
#endif