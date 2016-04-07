#ifndef LIVE_BEAUTIFUL
#define LIVE_BEAUTIFUL


#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <queue>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "My_Enhance.h"
#include "My_face_detection.h"
#include "My_Yuv_to_H264.h"
#include "My_SendH264To_Rtmp.h"
#include "CircleBuffer.h"


#pragma comment(lib,"opencv_core2410d.lib")                
#pragma comment(lib,"opencv_highgui2410d.lib")                
#pragma comment(lib,"opencv_objdetect2410d.lib")   
#pragma comment(lib,"opencv_imgproc2410d.lib")  
#pragma comment(lib,"libx264-132.lib")

#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib,"librtmp.lib")


extern CPs_CircleBuffer* m_pCircleBuffer;

#endif