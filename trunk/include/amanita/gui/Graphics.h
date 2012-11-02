#ifndef _AMANITA_GUI_GRAPHICS_H
#define _AMANITA_GUI_GRAPHICS_H

/**
 * @file amanita/gui/Graphics.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 


#include <stdio.h>
#include <stdarg.h>
#ifdef USE_DD
#include <windows.h>
#include <ddraw.h>
#include <olectl.h> 
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#include <amanita/Geometry.h>
#include <amanita/gui/Image.h>



/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Application;
class Font;
#ifdef USE_DD
class FontEngine;
#endif
/** @endcond */


#ifdef USE_SDL
typedef SDL_Surface *Surface;
#endif
#ifdef USE_DD
typedef LPDIRECTDRAWSURFACE7 Surface;
#endif


/** This class loads and handles graphics, like java.awt.Graphics in the java language.
 * 
 * @ingroup gui */
class Graphics {
friend class Application;
private:
	Application *app;							//!< 
#ifdef USE_DD
	HWND hwnd;
	HDC hdc;
	int width;
	int height;
	int bpp;
	int pitch;
	uint8_t *pixels;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAW7 dd;
	LPDIRECTDRAWCLIPPER lpclip;
	FontEngine *fontEngine;
	Surface front;
	Surface back;
#endif
#ifdef USE_SDL
	Surface screen;							//!< 
	Surface icon;								//!< 
#endif
	Surface canvas;							//!< 
	Font *font;									//!< 
	rect16_t cl;								//!< 

	void init(Application &a);
	void openDisplay(int w,int h,int d=0);
	void close();
#ifdef USE_DD
	void setHWND(HWND w);
	void eraseDesc();
#endif
#ifdef USE_SDL
	void setXPMIcon(const char **xpm);
	void setPNGIcon(const char *png);
#endif

public:
	Graphics();
	~Graphics();

	int getWidth();
	int getHeight();

	void setCanvas(Image *image) { setCanvas(image->surface); }
	void setCanvas(Image &image) { setCanvas(image.surface); }
	void setCanvas(Surface surface) { canvas = surface; }
	Surface getCanvas() { return canvas; }
	void resetCanvas();

	void lock();
	void unlock();
	void flip();

	int getPitch();
	int getBytesPerPixel();
	void *getPixels();

	void resetClip();
	void setClip(rect16_t &r);
	void setClip(int x,int y,int w,int h);
	rect16_t getClip() { return cl; }

	uint32_t mapRGB(uint32_t c) { return mapRGB((c>>16)&0xff,(c>>8)&0xff,c&0xff); }
	uint32_t mapRGB(uint8_t r,uint8_t g,uint8_t b);
	void pixel(int x,int y,uint32_t c);
	void draw(int x,int y,Image *s,int i) { draw(x,y,s->surface,s->getCell(i)); }
	void draw(int x,int y,Image &s,int i) { draw(x,y,s.surface,s.getCell(i)); }
	void draw(int x,int y,Image *s,rect16_t *src) { draw(x,y,s->surface,src); }
	void draw(int x,int y,Image &s,rect16_t *src) { draw(x,y,s.surface,src); }
	void draw(int x,int y,Image *s,rect16_t &src) { draw(x,y,s->surface,&src); }
	void draw(int x,int y,Image &s,rect16_t &src) { draw(x,y,s.surface,&src); }
	void draw(int x,int y,Surface s,rect16_t *src);
	void drawLine(int x1,int y1,int x2,int y2,uint32_t c);
	void drawCircle(int x,int y,int r,uint32_t c) {}
	void drawEllipse(int x,int y,int w,int h,uint32_t c) {}
	void drawDottedEllipse(int x,int y,int w,int h,uint32_t *p) {}
	void drawRect(int x,int y,int w,int h,uint32_t c) {}
	void drawDottedRect(int x,int y,int w,int h,uint32_t *p);
	void fillEllipse(int x,int y,int w,int h,uint32_t c) {}
	void fillRect(int x,int y,int w,int h,uint32_t c);
	void floodFill(int x,int y,uint32_t c,int mode) {}

	void blendLine(int x1,int y1,int x2,int y2,unsigned short c) {}
	void blendCircle(int x,int y,int r,unsigned short c) {}
	void blendRect(int x,int y,int w,int h,unsigned short c) {}
	void blendFillRect(int x,int y,int w,int h,unsigned short c) {}

	bool screenDump(const char *fn);
};


extern Graphics g;


}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_GRAPHICS_H */
