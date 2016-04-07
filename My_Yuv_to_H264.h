#ifndef MYYUVTOH264
#define MYYUVTOH264



#include <iostream>
#include <string>
#include "stdint.h"


//#include <time.h>
extern "C"
{
#include "x264.h"
#include "x264_config.h"
};

using namespace std;

int MyYuvtoH264(int width,int height,string filename);
int MyOneFrameYuvtoH264(int width,int height,char* yuvbuffer);


#endif