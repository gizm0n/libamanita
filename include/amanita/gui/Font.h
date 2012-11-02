#ifndef _AMANITA_GUI_FONT_H
#define _AMANITA_GUI_FONT_H

/**
 * @file amanita/gui/Font.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif
#include <amanita/Object.h>
#include <amanita/Fontmetrics.h>
#include <amanita/gui/Graphics.h>


/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


enum {
	ALIGN_LEFT			= 0x01,
	ALIGN_CENTER		= 0x02,
	ALIGN_RIGHT			= 0x04,
	ALIGN_TOP			= 0x08,
	ALIGN_MIDDLE		= 0x10,
	ALIGN_BOTTOM		= 0x20
};


enum {
#ifdef USE_DD
	FONT_NORMAL			= 0,
#endif
#ifdef USE_SDL
	FONT_NORMAL			= TTF_STYLE_NORMAL,
#endif
};


/** A class for handling fonts.
 * 
 * @ingroup gui */
class Font : public Object {
/** @cond */
Object_Instance(Font)
/** @endcond */

private:
	char *name;
	Fontmetrics metrics;
	Surface glyphs[256];

public:

	Font(const char *file,int size,int style=FONT_NORMAL,long color=0,int render=0);
	~Font();

	const char *getName() { return name; }
	Surface getGlyphSurface(int c) { return glyphs[c]; }
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

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_FONT_H */

