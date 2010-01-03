#ifndef _LIBAMANITA_SDL_DISPLAY_H
#define _LIBAMANITA_SDL_DISPLAY_H

/**
 * @file libamanita/sdl/aDisplay.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aComponent.h>


class aDisplay : public aComponent,public aKeyListener,public aMouseListener,public aMouseMotionListener,public aActionListener {
aObject_Instance(aDisplay)

private:
	static aDisplay *activeDisplay;
	SDL_Rect screen;

protected:
	static void setActiveDisplay(aDisplay *d) { activeDisplay = d; }
	void setScreen(int x,int y,int w,int h);

public:
	aDisplay();
	~aDisplay();

	static aDisplay *getActiveDisplay() { return activeDisplay; }

	int screenX() { return screen.x; }
	int screenY() { return screen.y; }
	int screenW() { return screen.w; }
	int screenH() { return screen.h; }

	virtual void start() { setActiveDisplay(this);setKeyFocus(this); }
	virtual void stop() {}

	virtual bool actionPerformed(aActionEvent &ae) { return false; }
};



#endif /* _LIBAMANITA_SDL_DISPLAY_H */

