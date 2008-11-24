#ifndef _LIBAMANITA_SDL_FONT_H
#define _LIBAMANITA_SDL_FONT_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <libamanita/Object.h>


enum {
	ALIGN_LEFT			= 0x01,
	ALIGN_CENTER		= 0x02,
	ALIGN_RIGHT			= 0x04,
	ALIGN_TOP			= 0x08,
	ALIGN_MIDDLE		= 0x10,
	ALIGN_BOTTOM		= 0x20
};


class Font : public Object {
friend class Text;

RttiObjectInstance(Font)

private:
	struct glyph {
		int minx,maxx,miny,maxy,advance;
		SDL_Surface *surface;
	};

	char *name;
	glyph glyphs[256];
	int height,ascent,descent,lineskip,adjx,adjy,spacing;

public:

	Font(const char *file,int size,int style=TTF_STYLE_NORMAL,long color=0,int render=0);
	~Font();

	const char *getName() { return name; }
	int getHeight() { return height; }
	int getAscent() { return ascent; }
	int getDescent() { return descent; }
	int getLineSkip() { return lineskip; }
	void setAdjust(int x,int y) { adjx = x,adjy = y; }
	void setSpacing(int s) { spacing = s; }
	int stringWidth(const char *str);
	int stringWidth(const char *str,int i,int l);
	void putchar(int x,int y,unsigned char c);
	void print(int x,int y,const char *str);
	void print(int x,int y,int a,const char *str);
	void printf(int x,int y,const char *format, ...);
	void printf(int x,int y,int a,const char *format, ...);
};




#endif /* _LIBAMANITA_SDL_FONT_H */

