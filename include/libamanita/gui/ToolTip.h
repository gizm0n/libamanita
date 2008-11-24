#ifndef _LIBAMANITA_GUI_TOOLTIP_H
#define _LIBAMANITA_GUI_TOOLTIP_H

#include <libamanita/gui/Component.h>

class Image;
class Font;

class ToolTip {
friend class Component;
protected:
	struct _settings {
		Image *img;
		Font *font;
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
	ToolTip();
	ToolTip(const char *text);
	virtual ~ToolTip();

	/**
	 * Image *img - The image should contain all the images mapped on the same image
	 * Image *ttimg - The image should contain all the tooltipimages mapped on the same image
	 * Font *f - Default font used for tooltip
	 * Uint32 data[35] - The array should be mapped as follows:
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
	static void setDefaultSettings(Image *img,Font *f,Uint32 data[13]);

	void setText(const char *text);
	virtual bool paintToolTip(ToolTipEvent &tte);
	static void paintToolTipRect(int x,int y,int w,int h);
};


#endif /* _LIBAMANITA_GUI_TOOLTIP_H */
