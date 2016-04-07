#include "stdafx.h"
#include "live_beautiful_camera_streaming.h"
#include "My_Yuv_to_H264.h"
#include "CircleBuffer.h"


//这个是编码文件的
int MyYuvtoH264(int width,int height,string filename)
{
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
	//设置视频的宽度、高度
	
///////////////////////////////////////////////////////////
	int fps = 25;//设置帧率
	size_t yuv_size = width * height * 3 / 2;
	x264_t *encoder;
	x264_picture_t pic_in, pic_out;
	uint8_t *yuv_buffer;


	x264_param_t param;
	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads = 1;
	param.i_width = width;
	param.i_height = height;
	param.i_fps_num = fps;//视频数据帧率
	param.i_fps_den = 1;

	param.i_keyint_max = 25;
	param.b_intra_refresh = 1;

	param.b_annexb = 1;

	x264_param_apply_profile(&param, "baseline");
	encoder = x264_encoder_open(&param);

	x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

	yuv_buffer =(uint8_t*) malloc(yuv_size);

	pic_in.img.plane[0] = yuv_buffer;
	pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
	pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;


	int64_t i_pts = 0;

	x264_nal_t *nals;
	int nnal;
/////////////////////////////////////////////////////////////////////////////
	//读取yuv视频文件，即待转码yuv文件
	FILE *inf=fopen(filename.c_str(),"rb");
	 //* 创建文件,用于存储编码数据 
	FILE *outf=fopen("result1.h264","ab");
	if(NULL==inf)
	{
		return -1;
	}


	//clock_t start,end;
	//float cpuTime;
	//start=time(NULL);
	while (fread(yuv_buffer, 1, yuv_size,inf) > 0) 
	{
		pic_in.i_pts = i_pts++;
		x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);
		x264_nal_t *nal;
		for (nal = nals; nal < nals + nnal; nal++) 
		{
			fwrite( nal->p_payload, 1, nal->i_payload,outf);//写h264
		}
	}
	//end=time(NULL);
	//cpuTime=(float)(end-start);
	//printf("cuptime= %f mseconds\n",cpuTime);



	x264_encoder_close(encoder);
	
	fclose(inf);
	fclose(outf);
	free(yuv_buffer);
	return 0;

}

int MyOneFrameYuvtoH264(int width,int height,char* yuvbuffer)
{
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
	//设置视频的宽度、高度
	
///////////////////////////////////////////////////////////
	int fps = 25;//设置帧率
	size_t yuv_size = width * height * 3 / 2;
	x264_t *encoder;
	x264_picture_t pic_in, pic_out;
	uint8_t *yuv_buffer;


	x264_param_t param;
	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads = 1;
	param.i_width = width;
	param.i_height = height;
	param.i_fps_num = fps;//视频数据帧率
	param.i_fps_den = 1;

	param.i_keyint_max = 25;
	param.b_intra_refresh = 1;

	param.b_annexb = 1;

	x264_param_apply_profile(&param, "baseline");
	encoder = x264_encoder_open(&param);

	x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

	yuv_buffer =(uint8_t*) malloc(yuv_size);

	pic_in.img.plane[0] = yuv_buffer;
	pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
	pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;


	int64_t i_pts = 0;

	x264_nal_t *nals;
	int nnal;
/////////////////////////////////////////////////////////////////////////////

	int h264size = 0;
	memcpy(yuv_buffer,yuvbuffer,yuv_size);
	
		pic_in.i_pts = i_pts++;
		char* h264buffer = new char[yuv_size + 1024];//局部缓冲区
		x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);
		x264_nal_t *nal;
		for (nal = nals; nal < nals + nnal; nal++) 
		{
			
			memcpy(h264buffer + h264size,nal->p_payload,nal->i_payload);
			h264size = h264size + nal->i_payload;

		}


	x264_encoder_close(encoder);
	
	
	////	2、写
	if (m_pCircleBuffer->GetFreeSize() < CIC_READCHUNKSIZE)
	{
		Sleep(1000);
		//continue;
	}
	//m_pCircleBuffer->Write(&h264size,sizeof(int));
	m_pCircleBuffer->Write(h264buffer,h264size);
	
	free(yuv_buffer);
	delete[] h264buffer;
	return h264size;

}
