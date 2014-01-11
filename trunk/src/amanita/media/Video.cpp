/*
 * avconv -y -f image2 -i mov/f%05d.tif -vf "setpts=1.6*PTS" -vcodec mpeg4 -b 4000k -maxrate 4000k -bufsize 4000k -s 640x480 fractal.avi
 * avconv -y -i fractal.mp4 -s 800x600 -b 6000k -ab 240k fractal2.mp4
 * avconv -y -i fractal.mp4 -i multiverse3.mp3 -c:v copy -c:a libmp3lame -vol 128 -q:a 4 fractal2.mp4
 **/
#include <amanita/media/Video.h>
#ifdef AMANITA_VIDEO
#include "../_config.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

using namespace a;


static const char *err_msg[] = {
	"Not a valid error code",
	"Could not deduce output format from file extension: using MPEG",
	"Could not find suitable output format",
	"Memory error",
	"Could not alloc stream",
	"No codec",
	"Codec not found",
	"Could not open codec",
	"Could not open file",
	"Error while writing video frame",
};


bool Video::reg = true;

const char *Video::getErrorMessage(int msg) {
	return err_msg[msg>=1 && msg<=9? msg : 0];
}


Video::Video() {
	buf_width = 0;
	buf_height = 0;
	sc_width = 0;
	sc_height = 0;
	codec = 0;
	fmt = 0;
	oc = 0;
	st = 0;
	c = 0;
	pic_rgb = 0;
	pic_yuv = 0;
	out = 0;
	buf_out_sz = 0;
	buf_rgb = 0;
	buf_yuv = 0;
	buf_out = 0;
	img_convert_ctx = 0;
	filming = false;
}

Video::~Video() {
	if(filming) end();
}

int Video::begin(const char *fn,int w,int h,int ws,int hs) {
	if(reg) {
		av_register_all();
		avcodec_register_all();
		reg = false;
	}

	buf_width = w;
	buf_height = h;
	sc_width = ws<=0? w : ws;
	sc_height = hs<=0? h : hs;
	img_convert_ctx = sws_getContext(buf_width,buf_height,PIX_FMT_RGB24,sc_width,sc_height,PIX_FMT_YUV420P,SWS_BICUBIC,0,0,0);

debug_output("Creating file...\n");
	fmt = av_guess_format(0,fn,0);
	if(!fmt) {
		fprintf(stderr,"%s\n",err_msg[1]);
		fmt = av_guess_format("mpeg",0,0);
	}
	if(!fmt) return 2;
	oc = avformat_alloc_context();
	if(!oc) return 3;
	oc->oformat = fmt;
	snprintf(oc->filename,sizeof(oc->filename),"%s",fn);

	st = avformat_new_stream(oc,0);
	if(!st) return 4;
	c = st->codec;
	if(!c) return 5;
	c->codec_id = oc->oformat->video_codec;
fprintf(stderr,"\n\n\nCodec: %d\n",c->codec_id);
	c->codec_type = AVMEDIA_TYPE_VIDEO;
	c->bit_rate = sc_width*sc_height*24;
	c->width = sc_width;
	c->height = sc_height;
	c->time_base.num = 1;
	c->time_base.den = 25;
	c->gop_size = 12;
	c->pix_fmt = PIX_FMT_YUV420P;
	if(c->codec_id==CODEC_ID_MPEG2VIDEO) c->max_b_frames = 2;
	if(c->codec_id==CODEC_ID_MPEG1VIDEO) c->mb_decision = 2;
	if(oc->oformat->flags&AVFMT_GLOBALHEADER) c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	codec = avcodec_find_encoder(c->codec_id);
	if(!codec) return 6;
	if(avcodec_open2(c,codec,0)<0) return 7;
	if(!(fmt->flags&AVFMT_NOFILE) && avio_open(&oc->pb,fn,AVIO_FLAG_WRITE)<0) return 8;

debug_output("Writing header...\n");
	avformat_write_header(oc,0);

	pic_rgb = avcodec_alloc_frame();
	buf_rgb = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_RGB24,buf_width,buf_height));
	avpicture_fill((AVPicture*)pic_rgb,buf_rgb,PIX_FMT_RGB24,buf_width,buf_height);
	pic_yuv = avcodec_alloc_frame();
	buf_yuv = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P,sc_width,sc_height));
	avpicture_fill((AVPicture*)pic_yuv,buf_yuv,PIX_FMT_YUV420P,sc_width,sc_height);
	buf_out_sz = 0x200000;
	buf_out = (uint8_t *)av_malloc(buf_out_sz);

	filming = true;
	return 0;
}

void Video::end() {
	int i;
	filming = false;

	av_write_trailer(oc);
	av_free(buf_out);
	av_free(pic_yuv);
	av_free(buf_yuv);
	av_free(pic_rgb);
	av_free(buf_rgb);

	avcodec_close(c);

	for(i=0; i<(int)oc->nb_streams; ++i) {
		av_freep(&oc->streams[i]->codec);
		av_freep(&oc->streams[i]);
	}
	if(!(fmt->flags&AVFMT_NOFILE)) avio_close(oc->pb);
	av_free(oc);

	buf_width = 0;
	buf_height = 0;
	sc_width = 0;
	sc_height = 0;
	codec = 0;
	fmt = 0;
	oc = 0;
	st = 0;
	c = 0;
	pic_rgb = 0;
	pic_yuv = 0;
	out = 0;
	buf_out_sz = 0;
	buf_rgb = 0;
	buf_yuv = 0;
	buf_out = 0;
	img_convert_ctx = 0;
debug_output("Finished movie.\n");
}

int Video::addFrame() {
	int ret = 0;
	sws_scale(img_convert_ctx,pic_rgb->data,pic_rgb->linesize,0,buf_height,pic_yuv->data,pic_yuv->linesize);
	if(oc->oformat->flags&AVFMT_RAWPICTURE) {
		av_init_packet(&pkt);
		pkt.flags |= AV_PKT_FLAG_KEY;
		pkt.stream_index = st->index;
		pkt.data = (uint8_t *)pic_yuv;
		pkt.size = sizeof(AVPicture);
		ret = av_interleaved_write_frame(oc,&pkt);
	} else {
#if LIBAVCODEC_VERSION_MAJOR >= 54
		av_init_packet(&pkt);
		pkt.data = NULL;
		pkt.size = 0;
		out = avcodec_encode_video2(c,&pkt,pic_yuv,&ret);
		if(out>0) {
			if(c->coded_frame->key_frame) pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index = st->index;
			ret = av_interleaved_write_frame(oc,&pkt);
		}
#else
		out = avcodec_encode_video(c,buf_out,buf_out_sz,pic_yuv);
		if(out>0) {
			av_init_packet(&pkt);
			if((int)c->coded_frame->pts!=AV_NOPTS_VALUE)
				pkt.pts = av_rescale_q(c->coded_frame->pts,c->time_base,st->time_base);
			if(c->coded_frame->key_frame) pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index= st->index;
			pkt.data = buf_out;
			pkt.size = out;
			ret = av_interleaved_write_frame(oc,&pkt);
		}
#endif
	}
	if(ret!=0) return 9;
	return 0;
}



#endif /* AMANITA_VIDEO */
