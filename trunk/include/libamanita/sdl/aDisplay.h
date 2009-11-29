#ifndef _LIBAMANITA_GUI_DISPLAY_H
#define _LIBAMANITA_GUI_DISPLAY_H

#include <libamanita/sdl/Graphics.h>
#include <libamanita/gui/Component.h>


class Display : public Component,public KeyListener,public MouseListener,public MouseMotionListener,public ActionListener {
RttiObjectInstance(Display)

private:
	static Display *activeDisplay;
	SDL_Rect screen;

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



#endif /* _LIBAMANITA_GUI_DISPLAY_H */

