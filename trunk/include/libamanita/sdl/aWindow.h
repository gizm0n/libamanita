#ifndef _LIBAMANITA_GUI_WINDOW_H
#define _LIBAMANITA_GUI_WINDOW_H

#include <libamanita/sdl/Graphics.h>
#include <libamanita/sdl/Image.h>
#include <libamanita/gui/Component.h>

class Font;
class IconButton;


enum {
	TOP_CAPTION			= 1,
	BOTTOM_CAPTION		= 2,
	CLOSE_BUTTON		= 4,
};

class Window : public Component,public FocusListener,public MouseListener,public ActionListener {
RttiObjectInstance(Window)

protected:
	struct _settings {
		Image *img;
		Image *bgimg;
		Uint32 bgindex,bgcol;
		Font *font;
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
	Image *bgimg;
	Uint32 bgindex,bgcol;
	_settings::frame fr;
	IconButton *cb;
	const char *tcap,*bcap;
	Font *tf,*bf;

public:
	Window(int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~Window();

	/**
	 * @param img The image should contain all the windowimages mapped on the same image.
	 * @param bgimg Image used as default for drawing the dialog background, unless opaque is set. If not used, set to 0.
	 * @param f Font used as default for captions.
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
	 * 15 = Close Button, X indent points relative to right corner
	 * 16 = Close Button, Y indent points relative to right corner
	 * 17 = Close Button, plain image
	 * 18 = Close Button, active (mouse over) image
	 * 19 = Close Button, down (button pressed) image
	 * 20 = Close Button, disabled image
	 **/
	static void setDefaultSettings(Image *img,Image *bgimg,Font *f,Uint32 data[21]);

	void setStyle(int st);
	void setBackground(Image *img,int index=0) { bgimg = img,bgindex = index; }
	void setBackground(Image &img,int index=0) { bgimg = &img,bgindex = index; }
	void setBackground(Uint32 col) { bgcol = col; }
	Uint32 getBackground() { return bgcol; }
	void setTopCaption(const char *tc,Font *f=0) { tcap = tc,tf = f? f : _s.font; }
	void setBottomCaption(const char *bc,Font *f=0) { bcap = bc,bf = f? f : _s.font; }
	void moveToCenter();
	void add(Component *c) { Component::add(c);c->setFocusListener(this); }
	void remove(Component *c) { Component::remove(c);c->setFocusListener(0); }
	bool focusLost(FocusEvent &fe) { return !(isVisible() && isModal() && !contains(fe.source));  }
	virtual bool mouseDown(MouseEvent &me);
	virtual bool actionPerformed(ActionEvent &ae);

	virtual void paint(time_t time);
};


#endif /* _LIBAMANITA_GUI_WINDOW_H */
