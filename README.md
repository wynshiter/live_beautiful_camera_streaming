# live_beautiful_camera_streaming
In the windows operating system, using a regular USB camera, image acquisition and beauty, and the result is encoded into H264 format push RTMP server sent to live

##程序流程：

![](https://github.com/wynshiter/live_beautiful_camera_streaming/blob/master/process.jpg)

###1.图像采集
先从opencv（2.4.10版本）采集回来摄像头的图像，是一帧一帧的
每一帧图像是一个矩阵，opencv中的mat 数据结构。
 
###2.人脸的美化
人脸美化，我们用的皮肤检测，皮肤在颜色空间是特定的一个区域
检测到这个区域（感兴趣区域），完后对这个区域进行美化，就是滤波，主要是双边滤波和高斯滤波。
 
###3.视频格式转换以及编码
处理好的矩阵颜色空间是rgb的，需要转换成yuv，yuv的颜色空间就是可以播放的，类似我们电视上面看的画面，编码就是传输时候需要发送流，只个流相当于针对数据的压缩，将yuv格式的视频帧编码成h264的格式
Rgb转换成yuv，opencv实现（美颜在这一步实现）
Yuv转换成h264，x264实现
H264转换成rtmp流，libxrtmp实现

###4.发送给服务器进行直播
H264的流一般就可以播放了，但是针对目前的网络直播主要是将h264转换成rtmp流，用rtmp的服务器进行播放，这块我们主要用的是adobe media server 5这个服务器进行接受工作
 
###5.技术难点
1.将人脸美化转换为皮肤检测
		2.各种编码的转换
		3.缓冲区的控制，这块是一个读者写着模型

实现效果：
![](https://github.com/wynshiter/live_beautiful_camera_streaming/blob/master/result.jpg)

###部分代码：

···cpp

 #include "stdafx.h"
 #include "live_beautiful_camera_streaming.h"
 #include "CircleBuffer.h"
 using namespace std;
 using namespace cv;
 #define  GOLABLE_BUFFER_SIZE 1024*64
CPs_CircleBuffer* m_pCircleBuffer;

void CameraToH264(void *pcn) 
{

	CvCapture* capture;
	//VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return ; };
	//if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

	VideoCapture cap(0); //打开默认的摄像头号
	if(!cap.isOpened())  //检测是否打开成功
		return ;

	int w = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int h = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	int yuv_bufLen = w*h*3/2;
	unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];

	int index = 0;///

	for(;;)
	{
		Mat frame;
		cap >> frame; // 从摄像头中获取新的一帧
		//detectAndDisplay( frame );
		imshow("original image", frame);
		//按esc推出
		if(waitKey(40) == 27) break;
		//detectAndenhance(frame);
		//imshow("enhance image",equalizeIntensityHist(frame));
		Mat temp;
		//SkinRGB(&IplImage(frame),&IplImage(temp));
		//highlight(frame);
		MySkinEnhance(frame);
	

		/////////////////////////////////////////
		cv::Mat yuvImg;
		cv::cvtColor(frame, yuvImg, CV_BGR2YUV_I420);
		memcpy(pYuvBuf, yuvImg.data, yuv_bufLen*sizeof(unsigned char));
		MyOneFrameYuvtoH264(w,h,(char *)pYuvBuf);
	
	}
	//摄像头会在VideoCapture的析构函数中释放
	waitKey(0);

	delete[] pYuvBuf;
}

void H264ToRtmp(void *pcn) 
{
	Sleep(3000);
	My_SendH264To_Rtmp();
	
}

int main( void )
{
	m_pCircleBuffer = new CPs_CircleBuffer(GOLABLE_BUFFER_SIZE);
	
	HANDLE h_cameraToh264;
	h_cameraToh264 = (HANDLE)_beginthread((void(__cdecl *)(void *))CameraToH264,0,(void *)1);
	
	

	HANDLE h_h264ToRtmp;
	h_h264ToRtmp = (HANDLE)_beginthread((void(__cdecl *)(void *))H264ToRtmp,0,(void *)1);


	WaitForSingleObject(h_cameraToh264,INFINITE);
	WaitForSingleObject(h_h264ToRtmp,INFINITE);
	

	   Sleep(100);
	return 0;
}
···

##[blog:](http://blog.csdn.net/wangyaninglm/article/details/51056101)
