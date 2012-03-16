#ifndef _AMANITA_GFX_TOOLTIP_H
#define _AMANITA_GFX_TOOLTIP_H

/**
 * @file amanita/gfx/aGfxTooltip.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxComponent.h>

class aGfxImage;
class aGfxFont;

/** Tooltip.
 * @ingroup sdl
 */
class aGfxTooltip {
friend class aGfxComponent;
protected:
	struct _settings {
		aGfxImage *img;
		aGfxFont *font;
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
	aGfxTooltip();
	aGfxTooltip(const char *text);
	virtual ~aGfxTooltip();

	/**
	 * aGfxImage *img - The image should contain all the images mapped on the same image
	 * aGfxImage *ttimg - The image should contain all the tooltipimages mapped on the same image
	 * aGfxFont *f - Default font used for tooltip
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
	static void setDefaultSettings(aGfxImage *img,aGfxFont *f,uint32_t data[13]);

	void setText(const char *text);
	virtual bool paintToolTip(aTooltipEvent &tte);
	static void paintToolTipRect(int x,int y,int w,int h);
};


#endif /* _AMANITA_GFX_TOOLTIP_H */
