#include "stdafx.h"
#include "My_Enhance.h"

// add by shiter 2016/3/3

Mat equalizeChannelHist(const Mat & inputImage)  
{  
	if( inputImage.channels() >= 3 )  
	{  
		vector<Mat> channels;  
		split(inputImage, channels);  

		Mat B,G,R;  

		equalizeHist( channels[0], B );  
		equalizeHist( channels[1], G );  
		equalizeHist( channels[2], R );  

		vector<Mat> combined;  
		combined.push_back(B);  
		combined.push_back(G);  
		combined.push_back(R);  

		Mat result;  
		merge(combined, result);  

		return result;  
	}
	else{return inputImage;}

	return inputImage;  
}  


Mat equalizeIntensityHist(const Mat & inputImage)  
{  
	if(inputImage.channels() >= 3)  
	{  
		Mat ycrcb;  

		cvtColor(inputImage, ycrcb, COLOR_BGR2YCrCb);  

		vector<Mat> channels;  
		split(ycrcb, channels);  

		equalizeHist(channels[0], channels[0]);  

		Mat result;  
		merge(channels,ycrcb);  

		cvtColor(ycrcb, result, COLOR_YCrCb2BGR);  

		return result;  
	}  

	return Mat();  
}  


//皮肤检测1

void SkinRGB(IplImage* src,IplImage* dst)
{
	//RGB颜色空间
	//均匀照明：R>95,G>40,B>20,R-B>15,R-G>15
	//侧向照明:R>200,G>210,B>170,R-B<=15,R>B,G>B

	int height=src->height,width=src->width,channel=src->nChannels,step=src->widthStep;
	int b=0,g=1,r=2;
	//cvZero(dst);
	unsigned char* p_src=(unsigned char*)src->imageData;
	unsigned char* p_dst=(unsigned char*)dst->imageData;

	for(int j=0;j<height;j++)
	{
		for(int i=0;i<width;i++)
		{
			if((p_src[j*step+i*channel+r]>95 && p_src[j*step+i*channel+g]>40 && p_src[j*step+i*channel+b]>20 &&
				(p_src[j*step+i*channel+r]-p_src[j*step+i*channel+b])>15&&(p_src[j*step+i*channel+r]-p_src[j*step+i*channel+g])>15)||
				(p_src[j*step+i*channel+r]>200&&p_src[j*step+i*channel+g]>210&&p_src[j*step+i*channel+b]>170&&
				(p_src[j*step+i*channel+r]-p_src[j*step+i*channel+b])<=15&&p_src[j*step+i*channel+r]>p_src[j*step+i*channel+b]&&
				p_src[j*step+i*channel+g]>p_src[j*step+i*channel+b]))

				p_dst[j*width+i]=255;
		}
	}

	cvCopy(src,dst,p_dst);  
	cvShowImage("Skin Detect",dst);  
}

//皮肤检测,并针对皮肤进行增强，模糊
void MySkinEnhance(Mat &frame)
{
	//Mat input_image =frame;  
	Mat output_mask;  
	Mat output_image;  
	Mat mask;  
	//肤色椭圆  
	/*椭圆皮肤模型*/  
	Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);  
	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);  

	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1) );  

	if(frame.empty())  
		return ;  

	Mat ycrcb_image;  
	output_mask = Mat::zeros(frame.size(), CV_8UC1);  
	cvtColor(frame, ycrcb_image, CV_BGR2YCrCb); //首先转换成到YCrCb空间  

	for(int i = 0; i < frame.rows; i++) //利用椭圆皮肤模型进行皮肤检测  
	{  
		uchar* p = (uchar*)output_mask.ptr<uchar>(i);  
		Vec3b* ycrcb = (Vec3b*)ycrcb_image.ptr<Vec3b>(i);  
		for(int j = 0; j < frame.cols; j++)  
		{  
			if(skinCrCbHist.at<uchar>(ycrcb[j][1], ycrcb[j][2]) > 0)  
				p[j] = 255;  
		}  
	}     

	//morphologyEx(output_mask,output_mask,MORPH_CLOSE,element);  

	 //output_mask.setTo(0);  
		
		dilate(output_mask,output_mask,Mat(32,32,CV_8U),Point(-1,-1),2);
		//imwrite("dilate.jpg",dst);
	// output_image.setTo(0);  
		frame.copyTo(output_image, output_mask);  
	


		Mat enhance = output_image;
		medianBlur(output_image,enhance,11);
		//blur(enhance,enhance,Size(4,4),Point(-1,-1),4);
		//imshow("blur face",enhance);
		for(int i = 0; i < output_image.rows; i++) //
		{  
			uchar* p = (uchar*)output_mask.ptr<uchar>(i);  

			for(int j = 0; j < output_image.cols; j++)  
			{  
				//不是纯黑的,下面这个50就是控制比较暗的都算，后期这块的代码要改，影响效率
				if((enhance.at<Vec3b>(i,j)[0] >= 20) && (enhance.at<Vec3b>(i,j)[1] >= 20)&& (enhance.at<Vec3b>(i,j)[2] >= 20) ) 
				{
					
					frame.at<Vec3b>(i,j)[0] =  enhance.at<Vec3b>(i,j)[0];

					frame.at<Vec3b>(i,j)[1] = enhance.at<Vec3b>(i,j)[1];
					frame.at<Vec3b>(i,j)[2] = enhance.at<Vec3b>(i,j)[2];
				}
			
			}  
		}     
		// 图像融合
		//addWeighted(input_image, 0.95, enhance, 0.05, 0.0, input_image);  
		imshow("ouput image",frame);

}


//提高亮度对比度
void highlight(Mat &frame)
{
	Mat src,dst;  
	double alpha =1.5;  
	double beta = 20;  

	src = frame; 
	if(!src.data)  
	{  
		cout<<"Failed to load image!"<<endl;  
		return ;  
	}  



	//dst = Mat::zeros(src.size(),src.type());  
	for (int i = 0;i<src.rows;++i) 
	{
		//uchar* inData=src.ptr<uchar>(i);
		
		for(int j= 0;j<src.cols;++j) 
		{
			 
				/*src.at<Vec3b>(i,j)[0] = saturate_cast<uchar>(src.at<Vec3b>(i,j)[0]*alpha+beta); 
				src.at<Vec3b>(i,j)[1] = saturate_cast<uchar>(src.at<Vec3b>(i,j)[1]*alpha+beta); 
				src.at<Vec3b>(i,j)[2] = saturate_cast<uchar>(src.at<Vec3b>(i,j)[2]*alpha+beta); */
				src.at<Vec3b>(i,j)[0] = (src.at<Vec3b>(i,j)[0]*alpha+beta); 
				src.at<Vec3b>(i,j)[1] = (src.at<Vec3b>(i,j)[1]*alpha+beta); 
				src.at<Vec3b>(i,j)[2] = (src.at<Vec3b>(i,j)[2]*alpha+beta);
			
		}
	}

	namedWindow("Handled Image");  
	imshow("Handled Image",src);  
	//waitKey();  
}