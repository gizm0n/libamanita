#ifndef _AMANITA_SDL_DISPLAY_H
#define _AMANITA_SDL_DISPLAY_H

/**
 * @file amanita/sdl/aDisplay.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aGraphics.h>
#include <amanita/sdl/aComponent.h>

/** The aDisplay class is like a desktop in which all other windows, dialogs and components are placed.
 * @ingroup sdl
 */
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



#endif /* _AMANITA_SDL_DISPLAY_H */

