#ifndef _AMANITA_MEDIA_VIDEO_H
#define _AMANITA_MEDIA_VIDEO_H

/**
 * @file amanita/media/Video.h  
 * @author Per Löwgren
 * @date Modified: 2013-01-22
 * @date Created: 2013-01-22
 */ 

#include <amanita/Config.h>
#ifdef HAVE_AVCODEC
#define AMANITA_VIDEO

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}


/** Amanita Namespace */
namespace a {


class Video {
private:
	static bool reg;					//<! Whether to register avcodec

	int buf_width;
	int buf_height;
	int sc_width;
	int sc_height;
	AVCodec *codec;
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	AVStream *st;
	AVCodecContext *c;
	AVFrame *pic_rgb;
	AVFrame *pic_yuv;
	int out;
	int buf_out_sz;
	uint8_t *buf_rgb;
	uint8_t *buf_yuv;
	uint8_t *buf_out;
	AVPacket pkt;
	SwsContext *img_convert_ctx;
	bool filming;

public:
	static const char *getErrorMessage(int msg);

	Video();
	~Video();

	int begin(const char *fn,int w,int h,int ws=0,int hs=0);
	void end();

	int addFrame();

	uint8_t *getBuffer() { return buf_rgb; }
	size_t getBufferSize() { return (size_t)(buf_width*buf_height*3); }
	int getBufferWidth() { return buf_width; }
	int getBufferHeight() { return buf_height; }
	int getOutputWidth() { return sc_width; }
	int getOutputHeight() { return sc_height; }
};



}; /* namespace a */


#endif /* HAVE_AVCODEC */
#endif /* _AMANITA_MEDIA_VIDEO_H */

