#include <iostream>
#include "rtmp_pusher.h"

int main(){
	auto pusher = create_RTMP_pusher("rtmp://47.100.37.207/live", width, height, 25);
	cv::VideoCapture cap("rtsp://admin:witai515@192.168.1.231:554/h264/ch0/main/av_stream");
	cv::Mat image;
	while (cap.read(image)) {
		// do your things
		pusher->start(image);
		//cv::imshow("image", image);
		//cv::waitKey(1);
	}
	return 0;
}