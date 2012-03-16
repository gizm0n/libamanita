#ifndef _AMANITA_GFX_WINDOW_H
#define _AMANITA_GFX_WINDOW_H

/**
 * @file amanita/gfx/aGfxWindow.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxGraphics.h>
#include <amanita/gfx/aGfxImage.h>
#include <amanita/gfx/aGfxComponent.h>

class aGfxFont;
class aGfxIconButton;


enum {
	TOP_CAPTION			= 1,
	BOTTOM_CAPTION		= 2,
	CLOSE_BUTTON		= 4,
};

/** Base widow class, and all windows and dialogs inherit this class.
 * @ingroup sdl
 */
class aGfxWindow : public aGfxComponent,public aGfxFocusListener,public aGfxMouseListener,public aGfxActionListener {
aObject_Instance(aGfxWindow)

protected:
	struct _settings {
		aGfxImage *img;
		aGfxImage *bgimg;
		uint32_t bgindex,bgcol;
		aGfxFont *font;
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
	aGfxImage *bgimg;
	uint32_t bgindex,bgcol;
	_settings::frame fr;
	aGfxIconButton *cb;
	const char *tcap,*bcap;
	aGfxFont *tf,*bf;

public:
	aGfxWindow(int x=0,int y=0,int w=0,int h=0,int st=0);
	virtual ~aGfxWindow();

	/**
	 * @param img The image should contain all the windowimages mapped on the same image.
	 * @param bgimg aGfxImage used as default for drawing the dialog background, unless opaque is set. If not used, set to 0.
	 * @param f aGfxFont used as default for captions.
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
	 * 15 = Close aGfxButton, X indent points relative to right corner
	 * 16 = Close aGfxButton, Y indent points relative to right corner
	 * 17 = Close aGfxButton, plain image
	 * 18 = Close aGfxButton, active (mouse over) image
	 * 19 = Close aGfxButton, down (button pressed) image
	 * 20 = Close aGfxButton, disabled image
	 **/
	static void setDefaultSettings(aGfxImage *img,aGfxImage *bgimg,aGfxFont *f,uint32_t data[21]);

	void setStyle(int st);
	void setBackground(aGfxImage *img,int index=0) { bgimg = img,bgindex = index; }
	void setBackground(aGfxImage &img,int index=0) { bgimg = &img,bgindex = index; }
	void setBackground(uint32_t col) { bgcol = col; }
	uint32_t getBackground() { return bgcol; }
	void setTopCaption(const char *tc,aGfxFont *f=0) { tcap = tc,tf = f? f : _s.font; }
	void setBottomCaption(const char *bc,aGfxFont *f=0) { bcap = bc,bf = f? f : _s.font; }
	void moveToCenter();
	void add(aGfxComponent *c) { aGfxComponent::add(c);c->setFocusListener(this); }
	void remove(aGfxComponent *c) { aGfxComponent::remove(c);c->setFocusListener(0); }
	bool focusLost(aFocusEvent &fe) { return !(isVisible() && isModal() && !contains(fe.source));  }
	virtual bool mouseDown(aGfxMouseEvent &me);
	virtual bool actionPerformed(aGfxActionEvent &ae);

	virtual void paint(time_t time);
};


#endif /* _AMANITA_GFX_WINDOW_H */
