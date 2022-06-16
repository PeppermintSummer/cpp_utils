#include <iostream>
#include "rtmp_pusher.h"

int main(){
	auto pusher = create_RTMP_pusher("rtmp://xxxxxxxxx/live", width, height, 25);
	cv::VideoCapture cap("rtsp://xxxx:xxx@xxx:554/h264/ch0/main/av_stream");
	cv::Mat image;
	while (cap.read(image)) {
		// do your things
		pusher->start(image);
		//cv::imshow("image", image);
		//cv::waitKey(1);
	}
	return 0;
}