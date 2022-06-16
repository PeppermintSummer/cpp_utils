#include "rtmp_pusher.h"
#include "logger.h"

class RTMP_pusher_Impl :public RTMP_pusher {
public:
	//explicit RTMP_pusher_Impl();
	bool startup(const std::string& output,
		int width, int height, int fps, int bitrate = 409600,
		int gop_size = 50, int max_b_frame = 0,
		bool log_enable = false) {
		url_ = output;
		width_ = width;
		height_ = height;
		fps_ = fps;
		bit_rate_ = bitrate;
		gop_size_ = gop_size;
		max_b_frame_ = max_b_frame;
		log_enable_ = log_enable;
		if (log_enable_) av_log_set_level(AV_LOG_DEBUG);

		//avcodec_register_all();
		//av_register_all();
		avformat_network_init();

		vsc = sws_getCachedContext(vsc,
			width_, height_, AV_PIX_FMT_BGR24,
			width_, height_, AV_PIX_FMT_YUV420P,
			SWS_BICUBIC,
			0, 0, 0);
		if (!vsc) return false;
		//�����
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = width_;
		yuv->height = height_;
		yuv->pts = 0;
		int ret = av_frame_get_buffer(yuv, 32);
		if (ret != 0) {
			return false;
		}
		//��ʼ������������
		AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) return false;
		vc = avcodec_alloc_context3(codec);
		if (!vc) return false;
		//���ñ���������
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		vc->codec_id = codec->id;
		vc->thread_count = 8;
		vc->bit_rate = bit_rate_;
		vc->width = width;
		vc->height = height;
		vc->time_base = { 1, fps };
		vc->framerate = { fps, 1 };
		vc->gop_size = gop_size_;
		vc->max_b_frames = max_b_frame_;
		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		av_opt_set(vc->priv_data, "tune", "zerolatency", 0);
		//�򿪱�����������
		ret = avcodec_open2(vc, 0, 0);
		if (ret != 0)return false;
		//�����װ��
		ret = avformat_alloc_output_context2(&ic, 0, "flv", url_.c_str());
		if (ret!=0) return false;
		vs = avformat_new_stream(ic, NULL);
		if (!vs) return false;
		vs->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(vs->codecpar, vc);//�ӱ��������Ʋ���
		av_dump_format(ic, 0, url_.c_str(), 1);
		//��rtmp���������IO
		ret = avio_open(&ic->pb, url_.c_str(), AVIO_FLAG_WRITE);
		if (ret != 0) return false;
		ret = avformat_write_header(ic, NULL);//д���װͷ
		if (ret != 0) return false;
		memset(&pack, 0, sizeof(pack));
		return true;
	}

	void start(cv::Mat& image) override {
		std::lock_guard<std::mutex>  lock_(mutex_);
		//RGB ==> YUV
		uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = image.data;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		insize[0] = image.cols * image.elemSize(); //���ֽ���
		int h = sws_scale(vsc, indata, insize, 0, image.rows,
			yuv->data, yuv->linesize);
		if (h <= 0)return;
		yuv->pts = vpts;
		vpts++;
		int ret = avcodec_send_frame(vc, yuv);
		if (ret != 0) {
			LOG_ERROR_("avcodec_send_frame() error.");
			return;
		}
		ret = avcodec_receive_packet(vc, &pack);
		//printf("packet.size() ==== %d \n", pack.size);
		if (pack.size == 0) {
			LOG_ERROR_("avcodec_receive_packet ERROR");
			return;
		}
		//����
		pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
		pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
		pack.duration = av_rescale_q(pack.duration, vc->time_base, vs->time_base);
		ret = av_interleaved_write_frame(ic, &pack);
		if (ret != 0) {
			LOG_ERROR_("av_interleaved_write_frame ERROR");
			return;
		}
	}
	void stop() {
		if (vsc != NULL) {
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (vc != NULL) {
			avio_closep(&ic->pb);
			avcodec_free_context(&vc);
		}
		if (thread_.joinable()) {
			thread_.join();
		}
	}
	~RTMP_pusher_Impl() {
		stop();
	}

private:
	//unique_ptr ===> std::move()
	SwsContext* vsc = NULL;
	AVFrame* yuv = NULL;
	AVCodecContext* vc = NULL;
	AVFormatContext* ic = NULL;
	AVPacket pack;
	AVStream* vs = NULL;

	std::string url_;
	std::mutex mutex_;
	std::queue<cv::Mat> frames;
	std::thread thread_;

	int width_;
	int height_;
	int fps_;
	int bit_rate_;
	int gop_size_;
	int max_b_frame_;
	bool log_enable_;
	int vpts = 0;
};

std::shared_ptr<RTMP_pusher> create_RTMP_pusher(const std::string& url,int width,int height,int fps) {
	std::shared_ptr<RTMP_pusher_Impl> instance(new RTMP_pusher_Impl());
	if (!instance->startup(url, width, height, fps)) {
		instance.reset();
	}
	return instance;
}