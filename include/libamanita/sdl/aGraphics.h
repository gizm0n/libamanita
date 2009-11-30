#ifndef _LIBAMANITA_SDL_AGRAPHICS_H
#define _LIBAMANITA_SDL_AGRAPHICS_H


#include <stdio.h>
#include <stdarg.h>
#include <SDL/SDL.h>
#include <libamanita/sdl/aImage.h>

class aFont;

class aGraphics {
private:
	SDL_Rect cl;
	SDL_Surface *screen,*canvas,*icon;
	aFont *font;

	static void initSDL(const char *caption,const char *ic1,SDL_Surface *ic2,int w,int h,int d,int f);

public:

	static void init(int w,int h,int d,int f) { initSDL(0,0,0,w,h,d,f); }
	static void init(const char *caption,int w,int h,int d,int f) { initSDL(caption,0,0,w,h,d,f); }
	static void init(const char *caption,const char *ic,int w,int h,int d,int f) { initSDL(caption,ic,0,w,h,d,f); }
	static void init(const char *caption,aImage *ic,int w,int h,int d,int f) { initSDL(caption,0,ic->getSurface(),w,h,d,f); }
	static void init(const char *caption,aImage &ic,int w,int h,int d,int f) { initSDL(caption,0,ic.getSurface(),w,h,d,f); }
	static void init(const char *caption,SDL_Surface *ic,int w,int h,int d,int f) { initSDL(caption,0,ic,w,h,d,f); }
	static void close();

	static void setCaption(const char *caption) { SDL_WM_SetCaption(caption,caption); }
	static void setCaption(const char *title,const char *icon) { SDL_WM_SetCaption(title,icon); }

	aGraphics() : screen(0),canvas(0),icon(0),font(0) {}
	aGraphics(aImage *sc) : screen(sc->surface),canvas(sc->surface),icon(0),font(0) {}
	aGraphics(aImage &sc) : screen(sc.surface),canvas(sc.surface),icon(0),font(0) {}
	aGraphics(SDL_Surface *sc) : screen(sc),canvas(sc),icon(0),font(0) {}
	~aGraphics() {}

	int getScreenWidth() { return screen->w; }
	int getScreenHeight() { return screen->h; }

	void flip() { SDL_Flip(screen); }

	SDL_Surface *getScreen() { return screen; }
	SDL_PixelFormat *getScreenFormat() { return screen->format; }
	void setCanvas(aImage *image) { setCanvas(image->getSurface()); }
	void setCanvas(aImage &image) { setCanvas(image.getSurface()); }
	void setCanvas(SDL_Surface *surface) { canvas = surface; }
	SDL_Surface *getCanvas() { return canvas; }
	void resetCanvas() { canvas = screen; }

	void lock() { if(SDL_MUSTLOCK(canvas)) SDL_LockSurface(canvas); }
	void unlock() { if(SDL_MUSTLOCK(canvas)) SDL_UnlockSurface(canvas); }

	void resetClip() { cl = (SDL_Rect){ 0,0,screen->w,screen->h };SDL_SetClipRect(canvas,0); }
	void setClip(SDL_Rect &r) { cl = r;SDL_SetClipRect(canvas,&cl); }
	void setClip(int x,int y,int w,int h) { cl = (SDL_Rect){ x,y,w,h };SDL_SetClipRect(canvas,&cl); }
	SDL_Rect getClip() { return cl; }

	uint32_t mapRGB(uint32_t c) { return SDL_MapRGB(canvas->format,c>>16,c>>8,c); }
	uint32_t mapRGB(Uint8 r,Uint8 g,Uint8 b) { return SDL_MapRGB(canvas->format,r,g,b); }
	void pixel(int x,int y,uint32_t c);
	void draw(int x,int y,aImage *s,int i) { draw(x,y,s->surface,s->getCell(i)); }
	void draw(int x,int y,aImage &s,int i) { draw(x,y,s.surface,s.getCell(i)); }
	void draw(int x,int y,aImage *s,SDL_Rect *src) { draw(x,y,s->surface,src); }
	void draw(int x,int y,aImage &s,SDL_Rect *src) { draw(x,y,s.surface,src); }
	void draw(int x,int y,aImage *s,SDL_Rect &src) { draw(x,y,s->surface,&src); }
	void draw(int x,int y,aImage &s,SDL_Rect &src) { draw(x,y,s.surface,&src); }
	void draw(int x,int y,SDL_Surface *s,SDL_Rect *src);
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

extern aGraphics g;


#endif /* _LIBAMANITA_SDL_AGRAPHICS_H */
