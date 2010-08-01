#ifndef _AMANITA_SDL_TOOLTIP_H
#define _AMANITA_SDL_TOOLTIP_H

/**
 * @file amanita/sdl/aTooltip.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aComponent.h>

class aImage;
class aFont;

/** Tooltip.
 * @ingroup sdl
 */
class aTooltip {
friend class aComponent;
protected:
	struct _settings {
		aImage *img;
		aFont *font;
		SDL_Rect *tl,*t,*tr,*l,*r,*bl,*b,*br,*c;
		SDL_Rect ins;
	};
	static _settings _s;

	struct tooltip {
		size_t cnt;
		char *text,**lines,nlines;
		short x,y,w,h;
	} tt;

public:
	aTooltip();
	aTooltip(const char *text);
	virtual ~aTooltip();

	/**
	 * aImage *img - The image should contain all the images mapped on the same image
	 * aImage *ttimg - The image should contain all the tooltipimages mapped on the same image
	 * aFont *f - Default font used for tooltip
	 * uint32_t data[35] - The array should be mapped as follows:
	 *  0 = Top left corner of tooltip
	 *  1 = Top border of tooltip
	 *  2 = Top right of tooltip
	 *  3 = Left border of tooltip
	 *  4 = Right border of tooltip
	 *  5 = Bottom left corner of tooltip
	 *  6 = Bottom border of tooltip
	 *  7 = Bottom right corner of tooltip
	 *  8 = Center Background of tooltip
	 *  9 = Insets, Left
	 * 10 = Insets, Top
	 * 11 = Insets, Right
	 * 12 = Insets, Bottom
	 **/
	static void setDefaultSettings(aImage *img,aFont *f,uint32_t data[13]);

	void setText(const char *text);
	virtual bool paintToolTip(aTooltipEvent &tte);
	static void paintToolTipRect(int x,int y,int w,int h);
};


#endif /* _AMANITA_SDL_TOOLTIP_H */
