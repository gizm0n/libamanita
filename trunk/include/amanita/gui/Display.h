#ifndef _AMANITA_GUI_DISPLAY_H
#define _AMANITA_GUI_DISPLAY_H

/**
 * @file amanita/gui/Display.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/gui/Graphics.h>
#include <amanita/gui/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** The Display class is like a desktop in which all other windows, dialogs and components are placed.
 * @ingroup sdl
 */
class Display : public Widget,public KeyListener,public MouseListener,public MouseMotionListener,public ActionListener {
/** @cond */
Object_Instance(Display)
/** @endcond */

private:
	static Display *activeDisplay;
	rect16_t screen;

protected:
	static void setActiveDisplay(Display *d) { activeDisplay = d; }
	void setScreen(int x,int y,int w,int h);

public:
	Display();
	~Display();

	static Display *getActiveDisplay() { return activeDisplay; }

	int screenX() { return screen.x; }
	int screenY() { return screen.y; }
	int screenW() { return screen.w; }
	int screenH() { return screen.h; }

	virtual void start() { setActiveDisplay(this);setKeyFocus(this); }
	virtual void stop() {}

	virtual bool actionPerformed(ActionEvent &ae) { return false; }
};

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_DISPLAY_H */

