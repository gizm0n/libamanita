#ifndef _AMANITA_SDL_SCROLL_H
#define _AMANITA_SDL_SCROLL_H

/**
 * @file amanita/sdl/aScroll.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aComponent.h>


enum {
	SCROLL_VERT			= 0,		//!< Scroll vertically, default
	SCROLL_HORIZ		= 1,		//!< Scroll horizontally
	SCROLL_LEFT			= 2,		//!< Place vertical scrollbar on left
	SCROLL_TOP			= 4,		//!< Place vertical scrollbar on top
	SCROLL_RIGHT		= 0,		//!< Place vertical scrollbar on right, default
	SCROLL_BOTTOM		= 0,		//!< Place vertical scrollbar on bottom, default
};

class aImage;
class aIconButton;

/** A class for handling scrolling of content.
 * @ingroup sdl
 */
class aScroll : public aComponent,public aMouseMotionListener,public aActionListener {
aObject_Instance(aScroll)

private:
	struct _settings {
		aImage *img;
		struct button {
			SDL_Rect *size;
			int plain,active,down,disabled;
		} up,down,left,right,slide;
		SDL_Rect *vbar,*hbar;
	};
	static _settings _s;

	int style,val,min,max,inc;
	aIconButton *bmin,*bmax,*bslide;
	SDL_Rect *bar,slide;
	aActionEvent ae;

public:
	aScroll(int id,int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~aScroll();

	/**
	 * aImage *img - The image should contain all the scrollimages mapped on the same image
	 * uint32_t data[35] - The array should be mapped as follows:
	 *  0 = Up aButton, plain
	 *  1 = Up aButton, active
	 *  2 = Up aButton, down
	 *  3 = Up aButton, disabled
	 *  4 = Down aButton, plain
	 *  5 = Down aButton, active
	 *  6 = Down aButton, down
	 *  7 = Down aButton, disabled
	 *  8 = Left aButton, plain
	 *  9 = Left aButton, active
	 * 10 = Left aButton, down
	 * 11 = Left aButton, disabled
	 * 12 = Right aButton, plain
	 * 13 = Right aButton, active
	 * 14 = Right aButton, down
	 * 15 = Right aButton, disabled
	 * 16 = Slide aButton, plain
	 * 17 = Slide aButton, active
	 * 18 = Slide aButton, down
	 * 19 = Slide aButton, disabled
	 * 20 = Vertical Scrollbar
	 * 21 = Horizontal Scrollbar
	 **/
	static void setDefaultSettings(aImage *img,uint32_t data[22]);

	void setStyle(int st=0);
	void setValues(int v,int mn,int mx,int i=1);
	void setValue(int v);
	int getValue() { return val; }
	void positionSlide();
	bool mouseDrag(aMouseMotionEvent &mme);
	bool actionPerformed(aActionEvent &ae);
	void paint(time_t time);
};



#endif /* _AMANITA_SDL_SCROLL_H */
