
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

const char *colorSchemeNames[] = { "Daylight","Palette","Neon","Gold","Silver","Ebony","Blood","Lapiz Lazuli","Amethyst","Fire","Foliage" };
const char *colorSchemeChars = "DPNGSEBLAF";

enum {
	CC_DAYLIGHT,
	CC_PALETTE,
	CC_NEON,
	CC_GOLD,
	CC_SILVER,
	CC_EBONY,
	CC_BLOOD,
	CC_LAPIZ_LAZULI,
	CC_AMETHYST,
	CC_FIRE,
	CC_FOLIAGE,
	CC_SCHEMES,
};

const uint32_t colorSchemes16[] = {
	// Daylight 0-10
	0x0000,0x0013,0x9cdf,0xffff,0xffe0,
	0xf800,0xffe0,0xffff,0x9cdf,0xffff,0x0013,
	// Palette 11-54
	0x001f,0x04df,0x07ff,0x07f3,0x07e0,
	0x9fe0,0xffe0,0xfcc0,0xf800,0xf813,0xf81f,
	0x000c,0x018c,0x032c,0x0326,0x0320,
	0x3320,0x6320,0x6180,0x6000,0x6006,0x600c,
	0x0006,0x0106,0x0186,0x0184,0x0180,
	0x2180,0x3180,0x3100,0x3000,0x3004,0x3006,
	0x000c,0x018c,0x032c,0x0326,0x0320,
	0x3320,0x6320,0x6180,0x6000,0x6006,0x600c,
	// Neon 55-65
	0x001f,0x04df,0x07ff,0x07f3,0x07e0,
	0x9fe0,0xffe0,0xfcc0,0xf800,0xf813,0xf81f,
	// Gold 66-69
	0xffe0,0xfff3,0xffe0,0xfcc0,
	// Silver 70-71
	0x3186,0xffff,
	// Ebony 72-73
	0x0000,0x5186,
	// Blood 74-75
	0xc800,0x6000,
	// Lapiz Lazuli 76-77
	0x000c,0x0006,
	// Amethyst 78-79
	0xc819,0x600c,
	// Fire 80-81
	0xf800,0xffe0,
	// Foliage 82-87
	0x0180,0x07e0,0x0186,0x0666,0x6186,0x67e6,
};

const uint32_t colorSchemes32[] = {
	// Daylight 0-10
	0x000000,0x000099,0x9999ff,0xffffff,0xffff00,
	0xff0000,0xffff00,0xffffff,0x9999ff,0xffffff,0x000099,
	// Palette 11-54
	0x0000ff,0x0099ff,0x00ffff,0x00ff99,0x00ff00,
	0x99ff00,0xffff00,0xff9900,0xff0000,0xff0099,0xff00ff,
	0x000066,0x003366,0x006666,0x006633,0x006600,
	0x336600,0x666600,0x663300,0x660000,0x660033,0x660066,
	0x000033,0x002233,0x003333,0x003322,0x003300,
	0x223300,0x333300,0x332200,0x330000,0x330022,0x330033,
	0x000066,0x003366,0x006666,0x006633,0x006600,
	0x336600,0x666600,0x663300,0x660000,0x660033,0x660066,
	// Neon 55-65
	0x0000ff,0x0099ff,0x00ffff,0x00ff99,0x00ff00,
	0x99ff00,0xffff00,0xff9900,0xff0000,0xff0099,0xff00ff,
	// Gold 66-69
	0xffff00,0xffff9d,0xffff00,0xff9900,
	// Silver 70-71
	0x333333,0xffffff,
	// Ebony 72-73
	0x000000,0x553333,
	// Blood 74-75
	0xcc0000,0x660000,
	// Lapiz Lazuli 76-77
	0x000066,0x000033,
	// Amethyst 78-79
	0xcc00cc,0x660066,
	// Fire 80-81
	0xff0000,0xffff00,
	// Foliage 82-87
	0x003300,0x00ff00,0x003333,0x00cc33,0x663333,0x66ff33,
};

const int colorSchemeOffset[] = {
	0,11,55,66,70,72,74,76,78,80,82,
};

const int colorSchemeLength[] = {
	11,44,11,4,2,2,2,2,2,2,6
};

Fractal fractal(width,height,FRACT_THREADED);
Colorcycle cycle[3];
int cc[] = { 0,0,0 };
int bpp;
bool edges = true;

Thread render_thread;

static void cycles() {
	int bpp = g.getBytesPerPixel(),n;
	if(bpp==2 || bpp==4) {
		const uint32_t *c = bpp==2? colorSchemes16 : colorSchemes32;
		const int *o = colorSchemeOffset;
		const int *l = colorSchemeLength;
		cc[0] = n = rnd.uint32(CC_SCHEMES);
fprintf(stderr,"Background: %s\n",colorSchemeNames[n]);
		cycle[0].setScheme(&c[o[n]],l[n],rnd.int32(l[n]),2,2,bpp==2? 16 : 32);
		for(int i=1; i<3; ++i) {
			cc[i] = n = rnd.uint32(CC_SCHEMES);
fprintf(stderr,"Colour %d: %s\n",i,colorSchemeNames[n]);
			cycle[i].setScheme(&c[o[n]],l[n],rnd.int32(l[n]),rnd.int32(120)+20,rnd.int32(120)+20,bpp==2? 16 : 32);
		}
	}
}

static void render(void *data) {
fprintf(stderr,"Rendering...\n");
	int x,y,w = width*4,h = height*4;
	double scale = (double)height/(double)h;
	Fractal f(w,h,FRACT_THREADED,&fractal);
	uint64_t t;
	uint32_t bg[height*sizeof(uint32_t)];
	uint32_t fg[ITER_MAX*2*sizeof(uint32_t)];
	app.lock();
	cycle[0].writeColors(bg,h,scale);
	cycle[1].writeColors(fg,ITER_MAX);
	cycle[2].writeColors(&fg[ITER_MAX],ITER_MAX);
	app.unlock();
	f.render(1024);
	while(f.getPercent()!=100) render_thread.pause(100);
fprintf(stderr,"Render finished\n");
	f.flip();
	{
		FILE *fp = fopen("fractal.ppm","wb");
		const char *comment = "# ";/* comment should start with # */
		const int MaxColorComponentValue = 255;
		uint8_t color[3072];
		uint16_t *fr,i;
		int n,c;
		fr = f.getFractal();
		fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,w,h,MaxColorComponentValue);
		for(y=0,c=0; y<h; ++y) {
			for(x=0; x<w; ++x) {
				i = fr[x+y*w];
				n = (i&ITER_BG)? bg[y] : fg[i&ITER_ALT];
				color[c] = (n>>16)&0xff;
				color[c+1] = (n>>8)&0xff;
				color[c+2] = n&0xff;
				c += 3;
				if(c==3072) {
					fwrite(color,1,c,fp);
					c = 0;
				}
			}
		}
		if(c>0) fwrite(color,1,c,fp);
		fclose(fp);
	}
	render_thread.stop();
}

static void key(uint32_t sym,uint32_t mod,uint32_t unicode,bool down) {
fprintf(stderr,"key: [sym: 0x%x, mod: 0x%x unicode: %c]\n",sym,mod,unicode);
	if(down) {
		if(mod&(MOD_LSHIFT|MOD_RSHIFT))
			switch(sym) {
				case KEY_F1:fractal.setStyle(-1,-1,FE_PLAIN,-1);break;
				case KEY_F2:fractal.setStyle(-1,-1,FE_CIRCLE,-1);break;
				case KEY_F3:fractal.setStyle(-1,-1,FE_SQUARE,-1);break;
				case KEY_F4:fractal.setStyle(-1,-1,FE_STRIP,-1);break;
				case KEY_F5:fractal.setStyle(-1,-1,FE_HALFPLANE,-1);break;
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

			case KEY_1:fractal.setStyle(-1,-1,-1,DE_PLAIN);break;
			case KEY_2:fractal.setStyle(-1,-1,-1,DE_STRIPES);break;
			case KEY_3:fractal.setStyle(-1,-1,-1,DE_BINARY);break;
			case KEY_4:fractal.setStyle(-1,-1,-1,DE_FEATHERS);break;
			case KEY_5:fractal.setStyle(-1,-1,-1,DE_4);break;
			case KEY_6:fractal.setStyle(-1,-1,-1,DE_5);break;
			case KEY_7:fractal.setStyle(-1,-1,-1,DE_6);break;
			case KEY_8:fractal.setStyle(-1,-1,-1,DE_7);break;
			case KEY_9:fractal.setStyle(-1,-1,-1,DE_8);break;
			case KEY_0:fractal.setStyle(-1,-1,-1,DE_9);break;

//				case KEY_BCKSP:fractal.setStyle(-1,-1,-1,-1,1);break;

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

			case KEY_RETURN:
				cycles();
				fractal.random();
				break;
			case KEY_e:edges = !edges;break;
			case KEY_j:
				cycles();
				fractal.random(SET_JULIA);
				break;
			case KEY_m:
				cycles();
				fractal.random(SET_MANDELBROT);
				break;

			case KEY_c:cycles();break;
			case KEY_p:
				fprintf(stdout,"./fractal ");
				fractal.print(stdout);
				for(int i=0; i<3; ++i) {
					fputc(' ',stdout);
					fputc(colorSchemeChars[cc[i]],stdout);
					cycle[i].print(stdout);
				}
				fprintf(stdout,"\n");
				fflush(stdout);
				break;
			case KEY_r:
			{
				if(!render_thread.isRunning())
					render_thread.start(render);
				break;
			}
			case KEY_s:
				cycles();
				fractal.randomStyle();
				break;
			
			/*{
fprintf(stderr,"Ctrl-C\n");
				int l;
				char str[1024];
				l = fractal.print(str);
				for(int i=0; i<3; ++i) {
					str[l++] = ' ';
					str[l++] = colorSchemeChars[cc[i]];
					l += cycle[i].print(&str[l]);
				}
				str[l] = '\0';
				app.setClipboard(str);
				return;
			}*/
			case KEY_t:
				if(fractal.usingEdgeTracing()) fractal.disableEdgeTracing();
				else fractal.enableEdgeTracing();
				fractal.reset();
				break;
			/*case KEY_l:
			{
fprintf(stderr,"Ctrl-V\n");
				char *str = app.getClipboard(),*p;
				double x,y,z;
				fractal.setStyle(str);
				if((p=strchr(str,' '))) {
					x = strtod(p,&p);
					y = strtod(p,&p);
					z = strtod(p,&p);
					fractal.setZoom(x,y,z);
					if(*p) {
						int bpp = g.getBytesPerPixel(),i,n;
						if(bpp==2 || bpp==4) {
							char *p;
							const uint32_t *c = bpp==2? colorSchemes16 : colorSchemes32;
							const int *o = colorSchemeOffset;
							const int *l = colorSchemeLength;
							for(i=0; p && *p && i<3; ++i) {
								for(n=0; n<CC_SCHEMES && colorSchemeChars[n]!=*p; ++n);
								if(n<CC_SCHEMES) {
									cc[i] = n;
if(i==0) fprintf(stderr,"Background: %s\n",colorSchemeNames[n]);
else fprintf(stderr,"Colour %d: %s\n",i,colorSchemeNames[n]);
									cycle[i].setScheme(&c[o[n]],l[n],p+1,bpp==2? 16 : 32);
								}
								p = strchr(p,' ');
							}
						}
					}
				}
				return;
			}*/
		}
	}
}

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {
//fprintf(stderr,"mouse: x[%d] y[%d] button[%d] clicks[%d]\n",x,y,button,clicks);
	if(down && clicks==1 && fractal.getPercent()==100)
		fractal.zoomIn(x,y,button==1? 0.5 : 2.0);
}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {}

static void paint(void *data) {
	uint16_t *fr,f;
	int pitch,bpp,x,y,w,h,i;
	uint8_t *pixels,*p,*c;
	uint32_t n;
	uint32_t bg[height*sizeof(uint32_t)];
	uint32_t fg[ITER_MAX*2*sizeof(uint32_t)];
	rect16_t zoom;
	while(app.isRunning()) {
//fprintf(stderr,"paint 1.\n");
		app.lock();
		cycle[0].writeColors(bg,height);
		cycle[1].writeColors(fg,ITER_MAX);
		cycle[2].writeColors(&fg[ITER_MAX],ITER_MAX);
//fprintf(stderr,"paint 2.\n");
//fprintf(stderr,"paint 3.\n");
		fr = fractal.getFractal();
		g.lock();
//fprintf(stderr,"paint 4.\n");
		pixels = (uint8_t *)g.getPixels();
		pitch = g.getPitch();
		bpp = g.getBytesPerPixel();
//fprintf(stderr,"pitch: %d, bpp: %d\n",pitch,bpp);
		for(y=0; y<height; ++y)
			for(x=0; x<width; ++x) {
//fprintf(stderr,"x: %d, y: %d\n",x,y);
				p = &pixels[x*bpp+y*pitch];
				f = fr[x+y*width];
				if(edges) n = (f&ITER_BG)? bg[y] : (f&ITER_FILL)? 0xffffff : (f&ITER_EDGE)? 0x000000 : fg[f&ITER_ALT];
				else n = (f&ITER_BG)? bg[y] : fg[f&ITER_ALT];
				for(i=0,c=(uint8_t *)&n; i<bpp; ++i) p[i] = c[i];
			}

//fprintf(stderr,"paint 5.\n");
		fractal.getZoom(x,y,zoom);
//fprintf(stderr,"paint 6.\n");
		app.unlock();
		if(fractal.getPercent()!=100) {
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
		g.flip();
		app.pauseFPS(10);
	}
}

static void calc(void *data) {
	do {
		if(fractal.getPercent()==-1) {
			fractal.render();
			app.lock();
			fractal.flip();
			app.unlock();
			if(!app.isRunning()) app.start(paint);
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
	fractal.random();
	if(argc<8) cycles();
	if(argc>=5) {
		double x,y,z;
		fractal.setStyle(argv[1]);
		x = atof(argv[2]);
		y = atof(argv[3]);
		z = atof(argv[4]);
		fractal.setZoom(x,y,z);
		if(argc>=8) {
			int bpp = g.getBytesPerPixel(),i,n;
			if(bpp==2 || bpp==4) {
				char *p;
				const uint32_t *c = bpp==2? colorSchemes16 : colorSchemes32;
				const int *o = colorSchemeOffset;
				const int *l = colorSchemeLength;
				for(i=0; i<3; ++i) {
					for(n=0,p=argv[5+i]; n<CC_SCHEMES && colorSchemeChars[n]!=*p; ++n);
					if(n<CC_SCHEMES) {
						cc[i] = n;
if(i==0) fprintf(stderr,"Background: %s\n",colorSchemeNames[n]);
else fprintf(stderr,"Colour %d: %s\n",i,colorSchemeNames[n]);
						cycle[i].setScheme(&c[o[n]],l[n],p+1,bpp==2? 16 : 32);
					}
				}
			}
		}
	}
//	fractal.calcFractal(0);
//	app.start(paint);
	thread.start(calc);
	app.main();
	app.close();
	return 0;
}
