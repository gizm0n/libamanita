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
#ifdef HAVE_TIFF_H
#include <tiffio.h>
#endif
#ifdef HAVE_LIBAVCODEC_H
//#ifdef HAVE_AV_CONFIG_H
//#undef HAVE_AV_CONFIG_H
//#endif
extern "C" {
//#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/mathematics.h>
}
#endif
#include <amanita/File.h>
#include <amanita/Random.h>
#include <amanita/Thread.h>
#include <amanita/Colorcycle.h>
#include <amanita/Fractal.h>
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>

#ifdef USE_WIN32
#include "_resource.h"
#define amanita32_xpm 0
#endif
#ifdef USE_LINUX
#include "../icons/32x32/amanita.xpm"
#endif

using namespace a;
using namespace a::gui;



Application app(APP_SDL,"Fractal","Amanita Fractal Explorer");



const int width = 1024;
const int height = 768;


Fractal fractal(width,height,FRACT_EDGETRACE);
Colorcycle cycle[3];
int cc[] = { 0,0,0 };

Fractal *render_fractal = 0;
Thread render_thread;
int render_zoom;

static void cycles() {
	int bpp = g.getBytesPerPixel(),n;
	if(bpp==2 || bpp==4) {
		cc[0] = n = rnd.uint32(CC_SCHEMES);
		cycle[0].setScheme(n,0,2,2,bpp);
fprintf(stderr,"Background: %s\n",cycle[0].getSchemeName());
		for(int i=1; i<3; ++i) {
			cc[i] = n = rnd.uint32(CC_SCHEMES);
			cycle[i].setScheme(n,0,rnd.int32(120)+20,rnd.int32(120)+20,bpp);
fprintf(stderr,"Colour %d: %s\n",i,cycle[i].getSchemeName());
		}
	}
}


#ifdef HAVE_TIFF_H
void saveTIF(const char *fn,int w,int h,uint16_t *fr,uint32_t *bg,uint32_t *fg) {
fprintf(stderr,"Saving...\n");
	uint16_t m;
	int x,y,n;
	uint8_t *row = (uint8_t *)malloc(w*3*sizeof(uint8_t));
	if(!row) perror("");
	else {
		TIFF *tiff = TIFFOpen(fn,"w");
		if(tiff) {
			TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,w);
			TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,h);
			TIFFSetField(tiff,TIFFTAG_COMPRESSION,COMPRESSION_LZW);
			TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
			TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
			TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,8);
			TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,3);
			for(y=0; y<h; ++y) {
				for(x=0; x<w; ++x) {
					m = fr[x+y*w];
					n = m==0? bg[y] : fg[m];
					row[(x*3)] = (n>>16)&0xff;
					row[(x*3)+1] = (n>>8)&0xff;
					row[(x*3)+2] = n&0xff;
				}
				if(TIFFWriteScanline(tiff,row,y,w*3)!= 1) {
					fprintf(stderr,"Could not write image\n");
					break;
				}
			}
			TIFFClose(tiff);
		} else fprintf(stderr,"Could not open outgoing image.\n");
		free(row);
	}
}
#else
void savePPM(const char *fn,int w,int h,uint16_t *fr,uint32_t *bg,uint32_t *fg) {
fprintf(stderr,"Saving...\n");
	uint16_t m;
	int x,y,n;
	uint8_t *row = (uint8_t *)malloc(w*3*sizeof(uint8_t));
	if(!row) perror("");
	else {
		FILE *fp = fopen(fn,"wb");
		if(fp) {
			const char *comment = "# ";
			const int mccv = 255;
			fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,w,h,mccv);
			for(y=0; y<h; ++y) {
				for(x=0; x<w; ++x) {
					m = fr[x+y*w];
					n = m==0? bg[y] : fg[m];
					row[(x*3)] = (n>>16)&0xff;
					row[(x*3)+1] = (n>>8)&0xff;
					row[(x*3)+2] = n&0xff;
				}
				fwrite(row,1,w*3,fp);
			}
			fclose(fp);
		} else perror(fn);
		free(row);
	}
}
#endif

#ifdef HAVE_TIFF_H
void saveTIF(const char *fn,int w,int h,float *fr,uint32_t *bg,uint32_t *fg) {
fprintf(stderr,"Saving...\n");
	float m;
	int x,y,i,n;
	uint8_t *row = (uint8_t *)malloc(w*3*sizeof(uint8_t));
	if(!row) perror("");
	else {
		TIFF *tiff = TIFFOpen(fn,"w");
		if(tiff) {
			TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,w);
			TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,h);
			TIFFSetField(tiff,TIFFTAG_COMPRESSION,COMPRESSION_LZW);
			TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
			TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
			TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,8);
			TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,3);
			for(y=0; y<h; ++y) {
				for(x=0; x<w; ++x) {
					m = fr[x+y*w];
					if(m==0.0f) n = bg[y];
					else {
						i = (int)m;
						n = (int)((m*65536.0f)-(float)(i*65536));
						if(i<0) i = -i,n = blend24a(fg[(i&0x7fff)|0x8000],fg[((i+1)&0x7fff)|0x8000],n);
						else n = blend24a(fg[i&0x7fff],fg[(i+1)&0x7fff],n);
					}
					row[(x*3)] = (n>>16)&0xff;
					row[(x*3)+1] = (n>>8)&0xff;
					row[(x*3)+2] = n&0xff;
				}
				if(TIFFWriteScanline(tiff,row,y,w*3)!= 1) {
					fprintf(stderr,"Could not write image\n");
					break;
				}
			}
			TIFFClose(tiff);
		} else fprintf(stderr,"Could not open outgoing image.\n");
		free(row);
	}
}
#else
void savePPM(const char *fn,int w,int h,float *fr,uint32_t *bg,uint32_t *fg) {
fprintf(stderr,"Saving...\n");
	float m;
	int x,y,i,n;
	uint8_t *row = (uint8_t *)malloc(w*3*sizeof(uint8_t));
	if(!row) perror("");
	else {
		FILE *fp = fopen(fn,"wb");
		if(fp) {
			const char *comment = "# ";
			const int mccv = 255;
			fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,w,h,mccv);
			for(y=0; y<h; ++y) {
				for(x=0; x<w; ++x) {
					m = fr[x+y*w];
					if(m==0.0f) n = bg[y];
					else {
						i = (int)m;
						n = (int)((m*65536.0f)-(float)(i*65536));
						if(i<0) i = -i,n = blend24a(fg[(i&0x7fff)|0x8000],fg[((i+1)&0x7fff)|0x8000],n);
						else n = blend24a(fg[i&0x7fff],fg[(i+1)&0x7fff],n);
					}
					row[(x*3)] = (n>>16)&0xff;
					row[(x*3)+1] = (n>>8)&0xff;
					row[(x*3)+2] = n&0xff;
				}
				fwrite(row,1,w*3,fp);
			}
			fclose(fp);
		} else perror(fn);
		free(row);
	}
}
#endif

static void render(void *data) {
fprintf(stderr,"Rendering...\n");
	int w = width*4,h = height*4;
	double scale = (double)height/(double)h;
	Fractal f(w,h,FRACT_FLOAT,&fractal);
	uint32_t bg[h];
	uint32_t fg[0xffff];
	app.lock();
	cycle[0].write(bg,h,scale);
	cycle[1].write(fg,0x8000);
	cycle[2].write(&fg[0x8000],0x8000);
	app.unlock();
	render_fractal = &f;
	f.render(1024);
	while(f.getPercent()!=100) render_thread.pause(100);
	render_fractal = 0;
fprintf(stderr,"Render finished\n");
	f.flip();
#ifdef HAVE_TIFF_H
	saveTIF("fractal.tif",w,h,f.getFloat(),bg,fg);
#else
	savePPM("fractal.ppm",w,h,f.getFloat(),bg,fg);
#endif
	render_thread.stop();
}

static void key(uint32_t sym,uint32_t mod,uint32_t unicode,bool down) {
fprintf(stderr,"key: [sym: 0x%x, mod: 0x%x unicode: %c]\n",sym,mod,unicode);
	if(down) {
		if(mod&(MOD_LSHIFT|MOD_RSHIFT))
			switch(sym) {
				case KEY_F1:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
				case KEY_F2:fractal.setStyle(-1,-1,FE_LOGN,-1);break;
				case KEY_F3:fractal.setStyle(-1,-1,FE_ZRINB,-1);break;
				case KEY_F4:fractal.setStyle(-1,-1,FE_ABSZRB,-1);break;
				case KEY_F5:fractal.setStyle(-1,-1,FE_ABSZRB2,-1);break;
				case KEY_F6:fractal.setStyle(-1,-1,FE_ABSZIB,-1);break;
				case KEY_F7:fractal.setStyle(-1,-1,FE_ABSZIB2,-1);break;
				case KEY_F8:fractal.setStyle(-1,-1,FE_ABSZRIB,-1);break;
				case KEY_F9:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
				case KEY_F10:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
				case KEY_F11:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
				case KEY_F12:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
			}
		else if(mod&(MOD_LCTRL|MOD_RCTRL))
			switch(sym) {
				case KEY_1:fractal.setPlane(PL_MU);break;
				case KEY_2:fractal.setPlane(PL_INVMU);break;
				case KEY_3:fractal.setPlane(PL_INVMU25);break;
				case KEY_4:fractal.setPlane(PL_LAMBDA);break;
				case KEY_5:fractal.setPlane(PL_INVLAMBDA);break;
				case KEY_6:fractal.setPlane(PL_INVLAMBDA1);break;
				case KEY_7:fractal.setPlane(PL_INVMU14);break;
				case KEY_8:fractal.setPlane(PL_INVMU2);break;
				case KEY_9:fractal.setPlane(PL_LAMBDANEG);break;
				case KEY_0:fractal.setPlane(PL_LAMBDASWAP);break;

				case KEY_s:
				{
					int i;
					FILE *fp = fopen("fractal.txt","wb");
//					app.lock();
					fractal.write(fp);
					for(i=0; i<3; ++i) cycle[i].write(fp);
//					app.unlock();
					fclose(fp);
					break;
				}
				case KEY_l:
				{
					int i;
					FILE *fp = fopen("fractal.txt","rb");
//					app.lock();
					fractal.read(fp);
					for(i=0; i<3; ++i) cycle[i].read(fp);
//					app.unlock();
					fclose(fp);
					break;
				}
			}
		else switch(sym) {
			case KEY_F1:fractal.setStyle(-1,ET_CIRCLE,-1,-1);return;
			case KEY_F2:fractal.setStyle(-1,ET_RECTANGLE,-1,-1);return;
			case KEY_F3:fractal.setStyle(-1,ET_AND,-1,-1);return;
			case KEY_F4:fractal.setStyle(-1,ET_PICKOVER,-1,-1);return;
			case KEY_F5:fractal.setStyle(-1,ET_BIO,-1,-1);return;
			case KEY_F6:fractal.setStyle(-1,ET_BIO2RI,-1,-1);return;
			case KEY_F7:fractal.setStyle(-1,ET_REAL,-1,-1);return;
			case KEY_F8:fractal.setStyle(-1,ET_ABSREAL,-1,-1);return;
			case KEY_F9:fractal.setStyle(-1,ET_IMAGINARY,-1,-1);return;
			case KEY_F10:fractal.setStyle(-1,ET_MANHATTAN,-1,-1);return;
			case KEY_F11:fractal.setStyle(-1,ET_MANR,-1,-1);return;
			case KEY_F12:fractal.setStyle(-1,ET_CIRCLE,-1,-1);return;

			case KEY_1:fractal.setStyle(-1,-1,-1,DE_PLAIN);break;
			case KEY_2:fractal.setStyle(-1,-1,-1,DE_STRIPES);break;
			case KEY_3:fractal.setStyle(-1,-1,-1,DE_BINARY);break;
			case KEY_4:fractal.setStyle(-1,-1,-1,DE_FEATHERS);break;
			case KEY_5:fractal.setStyle(-1,-1,-1,DE_1);break;
			case KEY_6:fractal.setStyle(-1,-1,-1,DE_2);break;
			case KEY_7:fractal.setStyle(-1,-1,-1,DE_3);break;
			case KEY_8:fractal.setStyle(-1,-1,-1,DE_4);break;
			case KEY_9:fractal.setStyle(-1,-1,-1,DE_5);break;
			case KEY_0:fractal.setStyle(-1,-1,-1,DE_6);break;

			case KEY_GT:
			case KEY_UP:fractal.increaseRange(100);break;
			case KEY_LT:
			case KEY_DOWN:fractal.increaseRange(-100);break;
			case KEY_PGUP:
			case KEY_HOME:fractal.increaseRadius(1);break;
			case KEY_PGDN:
			case KEY_END:fractal.increaseRadius(-1);break;
			case KEY_PLUS:
			case KEY_KPADD:fractal.zoomIn();break;
			case KEY_MINUS:
			case KEY_KPSUB:fractal.zoomOut();break;

			case KEY_ESC:fractal.restart();break;
			case KEY_BCKSP:fractal.resetZoom();break;
			case KEY_RETURN:fractal.addZoomNode();break;
			case KEY_j:fractal.setSet(SET_JULIA);break;
			case KEY_m:fractal.setSet(SET_MANDELBROT);break;

			case KEY_c:cycles();break;
			case KEY_r:
			{
				if(!render_thread.isRunning())
					render_thread.start(render);
				break;
			}
			case KEY_z:fractal.startZoom();break;
			case KEY_s:
				cycles();
				fractal.randomStyle();
				break;
			case KEY_t:
				if(fractal.usingEdgeTracing()) fractal.disableEdgeTracing();
				else fractal.enableEdgeTracing();
				fractal.reset();
				break;
		}
	}
}

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {
//fprintf(stderr,"mouse: x[%d] y[%d] button[%d] clicks[%d]\n",x,y,button,clicks);
	if(down && clicks==1 && fractal.getPercent()==100)
		fractal.zoomIn(x,y,button==1? 0.5 : 1.0);
}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {}

static void paint(void *data) {
	uint16_t *fr,f;
//	uint8_t *map;
	int pitch,bpp,x,y,w,h,i,bar;
	uint8_t *pixels,*p,*c;
	uint32_t n;
	uint32_t bg[height];
	uint32_t fg[0xffff];
	rect16_t zoom;
	while(app.isRunning()) {
//fprintf(stderr,"paint 1.\n");
		app.lock();
		cycle[0].write(bg,height);
		cycle[1].write(fg,0x8000);
		cycle[2].write(&fg[0x8000],0x8000);
//fprintf(stderr,"paint 2.\n");
//fprintf(stderr,"paint 3.\n");
		fr = fractal.getIndex();
//		map = fractal.getMap();
		g.lock();
//fprintf(stderr,"paint 4.\n");
		pixels = (uint8_t *)g.getPixels();
		pitch = g.getPitch();
		bpp = g.getBytesPerPixel();
//fprintf(stderr,"pitch: %d, bpp: %d\n",pitch,bpp);
		if(fractal.getWidth()==width*2 && fractal.getHeight()==height*2) {
			for(y=0; y<height; ++y)
				for(x=0; x<width; ++x) {
//fprintf(stderr,"x: %d, y: %d\n",x,y);
					p = &pixels[x*bpp+y*pitch];
					f = fr[(x<<1)+((y*width)<<2)];
					n = f==0? bg[y] : fg[f];
					for(i=0,c=(uint8_t *)&n; i<bpp; ++i) p[i] = c[i];
				}
		} else {
			for(y=0; y<height; ++y)
				for(x=0; x<width; ++x) {
//fprintf(stderr,"x: %d, y: %d\n",x,y);
					p = &pixels[x*bpp+y*pitch];
					/*if(map && (f=map[x+y*width])) n = (f&2)? 0xffffff : (f&4)? 0xff0000 : (f&8)? 0x00ff00 : 0x000000;
					else */f = fr[x+y*width],n = f==0? bg[y] : fg[f];
					for(i=0,c=(uint8_t *)&n; i<bpp; ++i) p[i] = c[i];
				}
		}
//fprintf(stderr,"paint 5.\n");
		fractal.getZoom(x,y,zoom);
//fprintf(stderr,"paint 6.\n");
		app.unlock();

		bar = fractal.getPercent();
		if(bar<=0 || bar>=100) bar = 0;
		if(render_fractal) bar = render_fractal->getPercent();

		if(fractal.isZooming()) {
			g.drawLine(0,y,width,y,0x00ffff);
			g.drawLine(x,0,x,height,0x00ffff);
		} else if(fractal.getPercent()!=100) {
			g.drawLine(0,y,width,y,0x00ffff);
			g.drawLine(x,0,x,height,0x00ffff);
			x = zoom.x,y = zoom.y,w = zoom.w,h = zoom.h;
			if(y>=0 && y<height) g.drawLine(x<0? 0 : x,y,(x+w>width? width : x+w)-1,y,0xff00ff);
			if(x>=0 && x<width) g.drawLine(x,y<0? 0 : y,x,(y+h>height? height : y+h)-1,0xff00ff);
			if(y+h>=0 && y+h<height) g.drawLine(x<0? 0 : x,y+h-1,(x+w>width? width : x+w)-1,y+h-1,0xff00ff);
			if(x+w>=0 && x+w<width) g.drawLine(x+w-1,y<0? 0 : y,x+w-1,(y+h>height? height : y+h)-1,0xff00ff);
		}

//fprintf(stderr,"draw_line: 0 0 %d, %d\n",width,height);
//		g.drawLine(0,0,width,height,0x00ffff);

		g.unlock();

		if(bar>0 && bar<=100) g.fillRect(0,0,width*bar/100,10,0xff00ff);
		if(fractal.isZooming()) g.fillRect(0,height-10,width*fractal.getZoomPercent()/100,10,0xff00ff);

		g.flip();
		app.pauseFPS(10);
	}
}

#ifdef HAVE_LIBAVCODEC_H
void writeFrame(int w,int h,uint16_t *fr,uint32_t *bg,uint32_t *fg,uint8_t *buf) {
	uint16_t m;
	int x,y,i,n;
	for(y=0,i=0; y<h; ++y) {
		for(x=0; x<w; ++x,i+=3) {
			m = fr[x+y*w];
			n = m==0? bg[y] : fg[m];
			buf[i] = (n>>16)&0xff;
			buf[i+1] = (n>>8)&0xff;
			buf[i+2] = n&0xff;
		}
	}
}
#endif

static void calc(void *data) {
	do {
		if(fractal.isZooming()) {
			int w = width*2;
			int h = height*2;
			uint32_t *bg;
			uint32_t *fg;
			Colorcycle c1;
			Colorcycle c2;
			Colorcycle c3;
			char fn[256];
#ifdef HAVE_LIBAVCODEC_H
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
			SwsContext *img_convert_ctx = sws_getContext(w,h,PIX_FMT_RGB24,width,height,PIX_FMT_YUV420P,SWS_BICUBIC,0,0,0);
			int i,ret;
#endif
			bg = (uint32_t *)malloc(h*sizeof(uint32_t));
			fg = (uint32_t *)malloc(0xffff*sizeof(uint32_t));
			app.lock();
			fractal.setSize(w,h);
			c1.setScheme(cycle[0]);
			c2.setScheme(cycle[1]);
			c3.setScheme(cycle[2]);
			app.unlock();

#ifdef HAVE_LIBAVCODEC_H
			strcpy(fn,"fractal.mp4");
			fmt = av_guess_format(0,fn,0);
			if(!fmt) {
				printf("Could not deduce output format from file extension: using MPEG.\n");
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

			/*if(av_set_parameters(oc,0)<0) {
				fprintf(stderr, "Invalid output format parameters\n");
				exit(1);
			}
			av_dump_format(oc,0,fn,1);*/

//			st = add_video_stream(oc,oc->oformat->video_codec);

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

/*
//c->bit_rate_tolerance = 0;
c->rc_max_rate = 0;
c->rc_buffer_size = 0;
c->gop_size = 40;
c->max_b_frames = 3;
c->b_frame_strategy = 1;
c->coder_type = 1;
c->me_cmp = 1;
c->me_range = 16;
c->qmin = 10;
c->qmax = 51;
c->scenechange_threshold = 40;
c->flags |= CODEC_FLAG_LOOP_FILTER;
//c->me_method = ME_EPZS;
c->me_subpel_quality = 5;
c->i_quant_factor = 0.71;
c->qcompress = 0.6;
c->max_qdiff = 4;
//c->directpred = 1;
c->flags2 |= CODEC_FLAG2_FASTPSKIP;
*/
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
				if(avio_open(&oc->pb,fn,URL_WRONLY)<0) {
					fprintf(stderr, "Could not open '%s'\n",fn);
					exit(1);
				}

			avformat_write_header(oc,0);

			pic_rgb = avcodec_alloc_frame();
			buf_rgb = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_RGB24,w,h));
			avpicture_fill((AVPicture*)pic_rgb,buf_rgb,PIX_FMT_RGB24,w,h);
			pic_yuv = avcodec_alloc_frame();
			buf_yuv = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P,width,height));
			avpicture_fill((AVPicture*)pic_yuv,buf_yuv,PIX_FMT_YUV420P,width,height);
			buf_out_sz = 0x200000;
			buf_out = (uint8_t *)av_malloc(buf_out_sz);
#else
			if(!a::exists("tmp")) a::mkdir("tmp");
#endif
			while(app.isRunning() && fractal.isZooming()) {
				fractal.renderZoom(1024);
				app.lock();
				fractal.flip();
				app.unlock();
				c1.write(bg,h,false);
				c1.advance(10.0/(double)c->time_base.den);
				c2.write(fg,0x8000,false);
				c2.advance(10.0/(double)c->time_base.den);
				c3.write(&fg[0x8000],0x8000,false);
				c3.advance(10.0/(double)c->time_base.den);
#ifdef HAVE_LIBAVCODEC_H
				writeFrame(w,h,fractal.getIndex(),bg,fg,buf_rgb);
				sws_scale(img_convert_ctx,pic_rgb->data,pic_rgb->linesize,0,h,pic_yuv->data,pic_yuv->linesize);
				if(oc->oformat->flags&AVFMT_RAWPICTURE) {
					av_init_packet(&pkt);
					pkt.flags |= AV_PKT_FLAG_KEY;
					pkt.stream_index = st->index;
					pkt.data = (uint8_t *)pic_yuv;
					pkt.size = sizeof(AVPicture);
					ret = av_interleaved_write_frame(oc,&pkt);
				} else {
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
				}
				if(ret!=0) {
					fprintf(stderr, "Error while writing video frame\n");
					exit(1);
				}
#else
#ifdef HAVE_TIFF_H
				sprintf(fn,"tmp/f%05d.tif",index);
				saveTIF(fn,width,height,fractal.getIndex(),bg,fg);
#else
				sprintf(fn,"tmp/f%05d.ppm",index);
				savePPM(fn,width,height,fractal.getIndex(),bg,fg);
#endif
#endif
			}

#ifdef HAVE_LIBAVCODEC_H
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
#endif
			free(bg);
			free(fg);
			app.lock();
			fractal.setSize(width,height);
			app.unlock();
fprintf(stderr,"Finished movie.\n");
		} else if(fractal.getPercent()==-1) {
			fractal.render();
			app.lock();
			fractal.flip();
			app.unlock();
		} else app.pause(200);
	} while(app.isRunning());
}

int main(int argc,char *argv[]) {
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
	app.init(width,height);
#ifdef HAVE_LIBAVCODEC_H
	av_register_all();
	avcodec_register_all();
#endif
	fractal.random();
	cycles();
	app.start(paint);
	thread.start(calc);
	app.main();
	app.close();
	return 0;
}
