#ifndef _AMANITA_GUI_TOOLTIP_H
#define _AMANITA_GUI_TOOLTIP_H

/**
 * @file amanita/gui/Tooltip.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/gui/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Image;
class Font;
/** @endcond */


/** Tooltip.
 * 
 * @ingroup gui */
class Tooltip {
friend class Widget;

protected:
	struct _settings {
		Image *img;
		Font *font;
		rect16_t *tl,*t,*tr,*l,*r,*bl,*b,*br,*c;
		rect16_t ins;
	};
	static _settings _s;

	struct tooltip {
		size_t cnt;
		char *text,**lines,nlines;
		short x,y,w,h;
	} tt;

public:
	Tooltip();
	Tooltip(const char *text);
	virtual ~Tooltip();

	/**
	 * Image *img - The image should contain all the images mapped on the same image
	 * Image *ttimg - The image should contain all the tooltipimages mapped on the same image
	 * Font *f - Default font used for tooltip
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
	static void setDefaultSettings(Image *img,Font *f,uint32_t data[13]);

	void setText(const char *text);
	virtual bool paintToolTip(TooltipEvent &tte);
	static void paintToolTipRect(int x,int y,int w,int h);
};

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_TOOLTIP_H */
