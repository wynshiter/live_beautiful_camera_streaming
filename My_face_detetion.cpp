#include "stdafx.h"
#include "My_face_detection.h"



/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
string face_cascade_name = "haarcascade_frontalface_alt.xml";

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name_onlyface = "Capture - only Face";
string window_name_face = "Capture - Face ";
//rng=12345;

///对于四个通道图像的操作
void mapToMat(const cv::Mat &srcAlpha, cv::Mat &dest, int x, int y)
{
	int nc = 3;
	int alpha = 0;

	for (int j = 0; j < srcAlpha.rows; j++)
	{
		for (int i = 0; i < srcAlpha.cols*3; i += 3)
		{
			alpha = srcAlpha.ptr<uchar>(j)[i / 3*4 + 3];
			//alpha = 255-alpha;
			if(alpha != 0) //4通道图像的alpha判断
			{
				for (int k = 0; k < 3; k++)
				{
					// if (src1.ptr<uchar>(j)[i / nc*nc + k] != 0)
					if( (j+y < dest.rows) && (j+y>=0) &&
						((i+x*3) / 3*3 + k < dest.cols*3) && ((i+x*3) / 3*3 + k >= 0) &&
						(i/nc*4 + k < srcAlpha.cols*4) && (i/nc*4 + k >=0) )
					{
						dest.ptr<uchar>(j+y)[(i+x*nc) / nc*nc + k] = srcAlpha.ptr<uchar>(j)[(i) / nc*4 + k];
					}
				}
			}
		}
	}
}

/**
* @function detectAndDisplay
*/
void detectAndenhance( Mat &frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;
	Mat hatAlpha;

	//hatAlpha = imread("2.png",-1);//圣诞帽的图片

	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	//equalizeHist( frame_gray, frame_gray );
	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	for( size_t i = 0; i < faces.size(); i++ )
	{
		Rect face(faces[i].x,faces[i].y,faces[i].x + faces[i].width,faces[i].y + faces[i].height);
		cvSetImageROI(&IplImage(frame),face);

		

		// Do the porcess
		blur(frame,frame,Size(7,7),Point(-1,-1));
		//////////////////////////////////////////////
		cvResetImageROI( &IplImage(frame) );
		Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
		ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 2, 8, 0 );

		// line(frame,Point(faces[i].x,faces[i].y),center,Scalar(255,0,0),5);

		Mat faceROI = frame_gray( faces[i] );
		std::vector<Rect> eyes;

		imshow( window_name_onlyface, faceROI );
		
	}
	//-- Show what you got
	imshow( window_name_face, frame );
	//imwrite("merry christmas.jpg",frame);
}

/** @函数 detectAndDisplay */
void detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- 多尺寸检测人脸
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	for( int i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		Mat faceROI = frame_gray( faces[i] );
		std::vector<Rect> eyes;

		//-- 在每张人脸上检测双眼
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		for( int j = 0; j < eyes.size(); j++ )
		{
			Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
			int radius = cvRound( (eyes[j].width + eyes[i].height)*0.25 );
			circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
		}
	}
	//-- 显示结果图像
	imshow( window_name_face, frame );
}