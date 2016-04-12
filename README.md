# live_beautiful_camera_streaming
In the windows operating system, using a regular USB camera, image acquisition and beauty, and the result is encoded into H264 format push RTMP server sent to live

##程序流程：

![](https://github.com/wynshiter/live_beautiful_camera_streaming/blob/master/process.jpg)

1.图像采集
先从opencv（2.4.10版本）采集回来摄像头的图像，是一帧一帧的
每一帧图像是一个矩阵，opencv中的mat 数据结构。
 
2.人脸的美化
人脸美化，我们用的皮肤检测，皮肤在颜色空间是特定的一个区域
检测到这个区域（感兴趣区域），完后对这个区域进行美化，就是滤波，主要是双边滤波和高斯滤波。
 
3.视频格式转换以及编码
处理好的矩阵颜色空间是rgb的，需要转换成yuv，yuv的颜色空间就是可以播放的，类似我们电视上面看的画面，编码就是传输时候需要发送流，只个流相当于针对数据的压缩，将yuv格式的视频帧编码成h264的格式
Rgb转换成yuv，opencv实现（美颜在这一步实现）
Yuv转换成h264，x264实现
H264转换成rtmp流，libxrtmp实现

4.发送给服务器进行直播
H264的流一般就可以播放了，但是针对目前的网络直播主要是将h264转换成rtmp流，用rtmp的服务器进行播放，这块我们主要用的是adobe media server 5这个服务器进行接受工作
 
5.技术难点
1.将人脸美化转换为皮肤检测
2.各种编码的转换
3.缓冲区的控制，这块是一个读者写着模型

实现效果：
![](https://github.com/wynshiter/live_beautiful_camera_streaming/blob/master/result.jpg)
