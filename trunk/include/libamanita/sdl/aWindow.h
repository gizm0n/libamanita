#ifndef _LIBAMANITA_SDL_AWINDOW_H
#define _LIBAMANITA_SDL_AWINDOW_H

#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aImage.h>
#include <libamanita/sdl/aComponent.h>

class aFont;
class aIconButton;


enum {
	TOP_CAPTION			= 1,
	BOTTOM_CAPTION		= 2,
	CLOSE_BUTTON		= 4,
};

class aWindow : public aComponent,public aFocusListener,public aMouseListener,public aActionListener {
RttiObjectInstance(aWindow)

protected:
	struct _settings {
		aImage *img;
		aImage *bgimg;
		uint32_t bgindex,bgcol;
		aFont *font;
		struct frame {
			SDL_Rect *tl,*t,*tr,*l,*r,*bl,*b,*br;
		} fr,cap;
		struct button {
			int x,y;
			int plain,active,down,disabled;
		} cb;
	};
	static _settings _s;

	int style;
	aImage *bgimg;
	uint32_t bgindex,bgcol;
	_settings::frame fr;
	aIconButton *cb;
	const char *tcap,*bcap;
	aFont *tf,*bf;

public:
	aWindow(int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~aWindow();

	/**
	 * @param img The image should contain all the windowimages mapped on the same image.
	 * @param bgimg aImage used as default for drawing the dialog background, unless opaque is set. If not used, set to 0.
	 * @param f aFont used as default for captions.
	 * @param data The array should be mapped as follows:
	 *  0 = If backgroundimage is set to 0, then backgroundcolor else index of backgroundimage map
	 *  1 = Top left corner, plain
	 *  2 = Top left corner, caption
	 *  3 = Top border, plain
	 *  4 = Top border, caption
	 *  5 = Top right corner, plain
	 *  6 = Top right corner, caption
	 *  7 = Left border
	 *  8 = Right border
	 *  9 = Bottom left corner, plain
	 * 10 = Bottom left corner, caption
	 * 11 = Bottom border, plain
	 * 12 = Bottom border, caption
	 * 13 = Bottom right corner, plain
	 * 14 = Bottom right corner, caption
	 * 15 = Close aButton, X indent points relative to right corner
	 * 16 = Close aButton, Y indent points relative to right corner
	 * 17 = Close aButton, plain image
	 * 18 = Close aButton, active (mouse over) image
	 * 19 = Close aButton, down (button pressed) image
	 * 20 = Close aButton, disabled image
	 **/
	static void setDefaultSettings(aImage *img,aImage *bgimg,aFont *f,uint32_t data[21]);

	void setStyle(int st);
	void setBackground(aImage *img,int index=0) { bgimg = img,bgindex = index; }
	void setBackground(aImage &img,int index=0) { bgimg = &img,bgindex = index; }
	void setBackground(uint32_t col) { bgcol = col; }
	uint32_t getBackground() { return bgcol; }
	void setTopCaption(const char *tc,aFont *f=0) { tcap = tc,tf = f? f : _s.font; }
	void setBottomCaption(const char *bc,aFont *f=0) { bcap = bc,bf = f? f : _s.font; }
	void moveToCenter();
	void add(aComponent *c) { aComponent::add(c);c->setFocusListener(this); }
	void remove(aComponent *c) { aComponent::remove(c);c->setFocusListener(0); }
	bool focusLost(aFocusEvent &fe) { return !(isVisible() && isModal() && !contains(fe.source));  }
	virtual bool mouseDown(aMouseEvent &me);
	virtual bool actionPerformed(aActionEvent &ae);

	virtual void paint(time_t time);
};


#endif /* _LIBAMANITA_SDL_AWINDOW_H */
