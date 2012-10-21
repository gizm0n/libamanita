#ifndef _AMANITA_GFX_FONT_H
#define _AMANITA_GFX_FONT_H

/**
 * @file amanita/gfx/aGfxFont.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <amanita/Object.h>
#include <amanita/Fontmetrics.h>


enum {
	ALIGN_LEFT			= 0x01,
	ALIGN_CENTER		= 0x02,
	ALIGN_RIGHT			= 0x04,
	ALIGN_TOP			= 0x08,
	ALIGN_MIDDLE		= 0x10,
	ALIGN_BOTTOM		= 0x20
};


/** A class for handling fonts.
 * @ingroup sdl
 */
class aGfxFont : public Object {
Object_Instance(aGfxFont)

private:
	char *name;
	Fontmetrics metrics;
	SDL_Surface *glyphs[256];

public:

	aGfxFont(const char *file,int size,int style=TTF_STYLE_NORMAL,long color=0,int render=0);
	~aGfxFont();

	const char *getName() { return name; }
	SDL_Surface *getGlyphSurface(int c) { return glyphs[c]; }
	const Fontmetrics &getFontMetrics() { return metrics; }
	int getHeight() { return metrics.height; }
	int getAscent() { return metrics.ascent; }
	int getDescent() { return metrics.descent; }
	int getLineSkip() { return metrics.lineskip; }

	void setAdjust(int x,int y) { metrics.adjx = x,metrics.adjy = y; }
	void setSpacing(int s) { metrics.spacing = s; }

	int stringWidth(const char *str);
	int stringWidth(const char *str,int i,int l);

	void putchar(int x,int y,unsigned char c);
	void print(int x,int y,const char *str);
	void print(int x,int y,int a,const char *str);
	void printf(int x,int y,const char *format, ...);
	void printf(int x,int y,int a,const char *format, ...);
};




#endif /* _AMANITA_GFX_FONT_H */

