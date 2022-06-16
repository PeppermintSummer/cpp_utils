#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>

#include <opencv2/opencv.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>	
}


class RTMP_pusher {
public:
	virtual void start(cv::Mat& image) = 0;
};


std::shared_ptr<RTMP_pusher> create_RTMP_pusher(const std::string& url,int width,int height,int fps);
