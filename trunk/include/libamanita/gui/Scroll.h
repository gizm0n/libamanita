#ifndef _LIBAMANITA_GUI_SCROLL_H
#define _LIBAMANITA_GUI_SCROLL_H

#include <libamanita/gui/Component.h>


enum {
	SCROLL_VERT			= 0,		// Scroll vertically, default
	SCROLL_HORIZ		= 1,		// Scroll horizontally
	SCROLL_LEFT			= 2,		// Place vertical scrollbar on left
	SCROLL_TOP			= 4,		// Place vertical scrollbar on top
	SCROLL_RIGHT		= 0,		// Place vertical scrollbar on right, default
	SCROLL_BOTTOM		= 0,		// Place vertical scrollbar on bottom, default
};

class Image;
class IconButton;

class Scroll : public Component,public MouseMotionListener,public ActionListener {
RttiObjectInstance(Scroll)

private:
	struct _settings {
		Image *img;
		struct button {
			SDL_Rect *size;
			int plain,active,down,disabled;
		} up,down,left,right,slide;
		SDL_Rect *vbar,*hbar;
	};
	static _settings _s;

	int style,val,min,max,inc;
	IconButton *bmin,*bmax,*bslide;
	SDL_Rect *bar,slide;
	ActionEvent ae;

public:
	Scroll(int id,int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~Scroll();

	/**
	 * Image *img - The image should contain all the scrollimages mapped on the same image
	 * Uint32 data[35] - The array should be mapped as follows:
	 *  0 = Up Button, plain
	 *  1 = Up Button, active
	 *  2 = Up Button, down
	 *  3 = Up Button, disabled
	 *  4 = Down Button, plain
	 *  5 = Down Button, active
	 *  6 = Down Button, down
	 *  7 = Down Button, disabled
	 *  8 = Left Button, plain
	 *  9 = Left Button, active
	 * 10 = Left Button, down
	 * 11 = Left Button, disabled
	 * 12 = Right Button, plain
	 * 13 = Right Button, active
	 * 14 = Right Button, down
	 * 15 = Right Button, disabled
	 * 16 = Slide Button, plain
	 * 17 = Slide Button, active
	 * 18 = Slide Button, down
	 * 19 = Slide Button, disabled
	 * 20 = Vertical Scrollbar
	 * 21 = Horizontal Scrollbar
	 **/
	static void setDefaultSettings(Image *img,Uint32 data[22]);

	void setStyle(int st=0);
	void setValues(int v,int mn,int mx,int i=1);
	void setValue(int v);
	int getValue() { return val; }
	void positionSlide();
	bool mouseDrag(MouseMotionEvent &mme);
	bool actionPerformed(ActionEvent &ae);
	void paint(int time);
};



#endif /* _LIBAMANITA_GUI_SCROLL_H */
