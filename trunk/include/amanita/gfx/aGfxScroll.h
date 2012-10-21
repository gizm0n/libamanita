#ifndef _AMANITA_GFX_SCROLL_H
#define _AMANITA_GFX_SCROLL_H

/**
 * @file amanita/gfx/aGfxScroll.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxComponent.h>


enum {
	SCROLL_VERT			= 0,		//!< Scroll vertically, default
	SCROLL_HORIZ		= 1,		//!< Scroll horizontally
	SCROLL_LEFT			= 2,		//!< Place vertical scrollbar on left
	SCROLL_TOP			= 4,		//!< Place vertical scrollbar on top
	SCROLL_RIGHT		= 0,		//!< Place vertical scrollbar on right, default
	SCROLL_BOTTOM		= 0,		//!< Place vertical scrollbar on bottom, default
};

class aGfxImage;
class aGfxIconButton;

/** A class for handling scrolling of content.
 * @ingroup sdl
 */
class aGfxScroll : public aGfxComponent,public aGfxMouseMotionListener,public aGfxActionListener {
Object_Instance(aGfxScroll)

private:
	struct _settings {
		aGfxImage *img;
		struct button {
			SDL_Rect *size;
			int plain,active,down,disabled;
		} up,down,left,right,slide;
		SDL_Rect *vbar,*hbar;
	};
	static _settings _s;

	int style,val,min,max,inc;
	aGfxIconButton *bmin,*bmax,*bslide;
	SDL_Rect *bar,slide;
	aGfxActionEvent ae;

public:
	aGfxScroll(int id,int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~aGfxScroll();

	/**
	 * aGfxImage *img - The image should contain all the scrollimages mapped on the same image
	 * uint32_t data[35] - The array should be mapped as follows:
	 *  0 = Up aGfxButton, plain
	 *  1 = Up aGfxButton, active
	 *  2 = Up aGfxButton, down
	 *  3 = Up aGfxButton, disabled
	 *  4 = Down aGfxButton, plain
	 *  5 = Down aGfxButton, active
	 *  6 = Down aGfxButton, down
	 *  7 = Down aGfxButton, disabled
	 *  8 = Left aGfxButton, plain
	 *  9 = Left aGfxButton, active
	 * 10 = Left aGfxButton, down
	 * 11 = Left aGfxButton, disabled
	 * 12 = Right aGfxButton, plain
	 * 13 = Right aGfxButton, active
	 * 14 = Right aGfxButton, down
	 * 15 = Right aGfxButton, disabled
	 * 16 = Slide aGfxButton, plain
	 * 17 = Slide aGfxButton, active
	 * 18 = Slide aGfxButton, down
	 * 19 = Slide aGfxButton, disabled
	 * 20 = Vertical Scrollbar
	 * 21 = Horizontal Scrollbar
	 **/
	static void setDefaultSettings(aGfxImage *img,uint32_t data[22]);

	void setStyle(int st=0);
	void setValues(int v,int mn,int mx,int i=1);
	void setValue(int v);
	int getValue() { return val; }
	void positionSlide();
	bool mouseDrag(aGfxMouseMotionEvent &mme);
	bool actionPerformed(aGfxActionEvent &ae);
	void paint(time_t time);
};



#endif /* _AMANITA_GFX_SCROLL_H */
