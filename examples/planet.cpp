/*
 * avconv -y -f image2 -i mov/f%05d.tif -vf "setpts=1.6*PTS" -vcodec mpeg4 -b 4000k -maxrate 4000k -bufsize 4000k -s 640x480 fractal.avi
 * avconv -y -i fractal.mp4 -s 800x600 -b 6000k -ab 240k fractal2.mp4
 * avconv -y -i fractal.mp4 -i multiverse3.mp3 -c:v copy -c:a libmp3lame -vol 128 -q:a 4 fractal2.mp4
 **/
#include "../src/amanita/_config.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*#ifdef HAVE_LIBAVCODEC_H
//#ifdef HAVE_AV_CONFIG_H
//#undef HAVE_AV_CONFIG_H
//#endif
extern "C" {
//#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}
#endif*/
#include <amanita/File.h>
#include <amanita/Math.h>
#include <amanita/Random.h>
#include <amanita/Thread.h>
#include <amanita/Colorcycle.h>
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>
#include <amanita/media/Video.h>
#include <amanita/sim/Planet.h>

#ifdef USE_WIN32
#include "_resource.h"
#define amanita32_xpm 0
#endif
#ifdef USE_LINUX
#include "../icons/32x32/amanita.xpm"
#endif

using namespace a;
using namespace a::gui;



Application app(APP_SDL,"Planet","Planet Simulation");



//int width = 128;
//int height = 128;
int width = 640;
int height = 320;
//int width = 854;
//int height = 480;

bool is_filming = false;
bool is_rotating = true;


Planet planet;
int filter;


static void key(uint32_t sym,uint32_t mod,uint32_t unicode,bool down) {
fprintf(stderr,"key: [sym: 0x%x, mod: 0x%x unicode: %c]\n",sym,mod,unicode);
	if(down) {
		if(mod&(MOD_LSHIFT|MOD_RSHIFT))
			switch(sym) {
				case KEY_F1:break;
			}
		else if(mod&(MOD_LCTRL|MOD_RCTRL))
			switch(sym) {
				case KEY_1:break;

				case KEY_s:break;
			}
		else switch(sym) {
			case KEY_F1:return;

			case KEY_1:filter = 1;return;
			case KEY_2:filter = 2;return;
			case KEY_3:filter = 3;return;
			case KEY_4:filter = 4;return;
			case KEY_5:filter = 5;return;
			case KEY_0:filter = 0;return;

			case KEY_GT:
			case KEY_UP:break;
			case KEY_LT:
			case KEY_DOWN:break;
			case KEY_PGUP:
			case KEY_HOME:break;
			case KEY_PGDN:
			case KEY_END:break;
			case KEY_PLUS:
			case KEY_KPADD:break;
			case KEY_MINUS:
			case KEY_KPSUB:break;

			case KEY_ESC:break;
			case KEY_BCKSP:break;
			case KEY_RETURN:is_filming = !is_filming;break;
			case KEY_SPACE:is_rotating = !is_rotating;break;
			case KEY_TAB:break;
			case KEY_j:break;
		}
	}
}

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {
//fprintf(stderr,"mouse: x[%d] y[%d] button[%d] clicks[%d]\n",x,y,button,clicks);
}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {}

static void paint(void *data) {
	int pitch,bpp,x,y,tx,ix;
	uint8_t *pixels,*p;
	RGBA_Color col;

	while(app.isRunning()) {
//fprintf(stderr,"paint 1.\n");
		app.lock();

		g.lock();
		pixels = (uint8_t *)g.getPixels();
		pitch = g.getPitch();
		bpp = g.getBytesPerPixel();

		tx = planet.getTime()%width;
		if(!is_rotating) tx = 0;

//fprintf(stderr,"pitch: %d, bpp: %d\n",pitch,bpp);

		for(y=0; y<height; ++y) {
			for(x=tx,ix=0; ix<width; ++x,++ix) {
				if(x==width) x = 0;
				p = &pixels[ix*bpp+y*pitch];
				col = planet.getColor(x,y,filter);
				p[0] = col.b;
				p[1] = col.g;
				p[2] = col.r;
			}
		}
		app.unlock();
		g.unlock();
		g.flip();
		app.pauseFPS(10);
	}
}

static void calc(void *data) {
	do {
#ifdef AMANITA_VIDEO
//#ifdef HAVE_LIBAVCODEC_H
		if(is_filming) {
			RGBA_Color col;
			int x,y,i,tx,ix,err;
			uint8_t *buf;

			Video video;
			err = video.begin("planet.mp4",width,height);
			if(err!=0) {
				fprintf(stderr,"%s\n",video.getErrorMessage(err));
				is_filming = false;
				continue;
			}
			buf = video.getBuffer();

/*			char fn[256];
printf("Starting to film...\n");
			AVCodec *codec = 0;
			AVOutputFormat *fmt;
			AVFormatContext *oc;
			AVStream *st;
			AVCodecContext *c;
			AVFrame *pic_rgb,*pic_yuv;
			int out,buf_out_sz;
		//	FILE *fp;
			uint8_t *buf_rgb,*buf_yuv,*buf_out;
			AVPacket pkt;
			SwsContext *img_convert_ctx = sws_getContext(width,height,PIX_FMT_RGB24,width,height,PIX_FMT_YUV420P,SWS_BICUBIC,0,0,0);

printf("Creating file...\n");
			strcpy(fn,"planet.mp4");
			fmt = av_guess_format(0,fn,0);
			if(!fmt) {
				fprintf(stderr,"Could not deduce output format from file extension: using MPEG.\n");
				fmt = av_guess_format("mpeg",0,0);
			}
			if(!fmt) {
				fprintf(stderr,"Could not find suitable output format\n");
				exit(1);
			}
			oc = avformat_alloc_context();
			if(!oc) {
				fprintf(stderr,"Memory error\n");
				exit(1);
			}
			oc->oformat = fmt;
			snprintf(oc->filename,sizeof(oc->filename),"%s",fn);

			st = avformat_new_stream(oc,0);
			if(!st) {
				fprintf(stderr,"Could not alloc stream\n");
				exit(1);
			}
			c = st->codec;
			if(!c) {
				fprintf(stderr,"No codec\n");
				exit(1);
			}
			c->codec_id = oc->oformat->video_codec;
fprintf(stderr,"\n\n\nCodec: %d\n",c->codec_id);
			c->codec_type = AVMEDIA_TYPE_VIDEO;
			c->bit_rate = width*height*24;
			c->width = width;
			c->height = height;
			c->time_base.num = 1;
			c->time_base.den = 25;
			c->gop_size = 12;
			c->pix_fmt = PIX_FMT_YUV420P;
			if(c->codec_id==CODEC_ID_MPEG2VIDEO) c->max_b_frames = 2;
			if(c->codec_id==CODEC_ID_MPEG1VIDEO) c->mb_decision = 2;
			if(oc->oformat->flags&AVFMT_GLOBALHEADER) c->flags |= CODEC_FLAG_GLOBAL_HEADER;

			codec = avcodec_find_encoder(c->codec_id);
			if(!codec) {
				fprintf(stderr,"codec not found\n");
				exit(1);
			}
			if(avcodec_open2(c,codec,0)<0) {
				fprintf(stderr,"could not open codec\n");
				exit(1);
			}

			if(!(fmt->flags&AVFMT_NOFILE))
				if(avio_open(&oc->pb,fn,AVIO_FLAG_WRITE)<0) {
					fprintf(stderr, "Could not open '%s'\n",fn);
					exit(1);
				}

printf("Writing header...\n");
			avformat_write_header(oc,0);

			pic_rgb = avcodec_alloc_frame();
			buf_rgb = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_RGB24,width,height));
			avpicture_fill((AVPicture*)pic_rgb,buf_rgb,PIX_FMT_RGB24,width,height);
			pic_yuv = avcodec_alloc_frame();
			buf_yuv = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P,width,height));
			avpicture_fill((AVPicture*)pic_yuv,buf_yuv,PIX_FMT_YUV420P,width,height);
			buf_out_sz = 0x200000;
			buf_out = (uint8_t *)av_malloc(buf_out_sz);*/

printf("Running...\n");
			while(app.isRunning() && is_filming/* && !planet.isCompleted()*/) {

				planet.run();

				for(y=0,i=0,tx=planet.getTime()%width; y<height; ++y) {
					for(x=tx,ix=0; ix<width; ++x,++ix,i+=3) {
						if(x==width) x = 0;
						col = planet.getColor(x,y,filter);
						buf[i] = col.r;
						buf[i+1] = col.g;
						buf[i+2] = col.b;
					}
				}

				err = video.addFrame();
				if(err!=0) {
					fprintf(stderr,"%s\n",video.getErrorMessage(err));
					is_filming = false;
					break;
				}

/*
				sws_scale(img_convert_ctx,pic_rgb->data,pic_rgb->linesize,0,height,pic_yuv->data,pic_yuv->linesize);
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
					} else ret = 0;
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
					} else ret = 0;
#endif
				}
				if(ret!=0) {
					fprintf(stderr, "Error while writing video frame\n");
					exit(1);
				}*/
			}
			is_filming = false;
			video.end();

/*			av_write_trailer(oc);
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
printf("Finished movie.\n");*/
		} else {
#endif /* AMANITA_VIDEO */

			if(!planet.isCompleted()) planet.run();
			app.pauseFPS(20);

#ifdef AMANITA_VIDEO
//#ifdef HAVE_LIBAVCODEC_H
		}
#endif /* AMANITA_VIDEO */
	} while(app.isRunning());
}

int main(int argc,char *argv[]) {
	int i,f;
	char *p1,*p2;
//fprintf(stderr,"main\n");
	Thread thread;
	const WindowIcon icons[] = {
		{ 16,AMANITA_ICON,0,0 },
		{ 32,0,amanita32_xpm,0 },
		{ 48,AMANITA_ICON,0,0 },
	{0}};
	app.setIcons(icons);
	app.setEvents(key,mouse,motion);
	app.open(argc,argv);

	for(i=1; i<argc; ++i) {
		f = 0,p1 = 0,p2 = 0;
		if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) printf(
			"Usage: planet [OPTION]...\n"
			"  -h, --help                 Show help options\n"
			"  -s, --size=SIZE            Size of planet: 640x320 (default)\n");
		else if(!strncmp(argv[i],"-s=",3)) p1 = argv[i]+3,f = 1;
		else if(!strncmp(argv[i],"--size=",7)) p1 = argv[i]+7,f = 1;
		if(f) switch(f) {
			case 1:
				if(p1) {
					p2 = strchr(p1,'x');
					if(!p2) p2 = strchr(p1,'X');
					if(p2) {
						++p2;
						width = atoi(p1);
						height = atoi(p2);
						printf("Set size to: %dx%d\n",width,height);
					}
				}
				break;
		}
	}

	app.init(width,height);
#ifdef HAVE_LIBAVCODEC_H
	av_register_all();
	avcodec_register_all();
#endif

	planet.create(
		width,	
		height,	
		5,			// Continents N
		40,		// Landmass %
		30,		// Mountains %
		20,		// Islands %
		50,		// Forest %
		50,		// Population %
		50			// Technology %
	);
	app.start(paint);
	thread.start(calc,0,0);
	app.main();
	app.close();
	return 0;
}
