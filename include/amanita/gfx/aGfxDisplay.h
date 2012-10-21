#ifndef _AMANITA_GFX_DISPLAY_H
#define _AMANITA_GFX_DISPLAY_H

/**
 * @file amanita/gfx/aGfxDisplay.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxGraphics.h>
#include <amanita/gfx/aGfxComponent.h>

/** The aGfxDisplay class is like a desktop in which all other windows, dialogs and components are placed.
 * @ingroup sdl
 */
class aGfxDisplay : public aGfxComponent,public aGfxKeyListener,public aGfxMouseListener,public aGfxMouseMotionListener,public aGfxActionListener {
Object_Instance(aGfxDisplay)

private:
	static aGfxDisplay *activeDisplay;
	SDL_Rect screen;

protected:
	static void setActiveDisplay(aGfxDisplay *d) { activeDisplay = d; }
	void setScreen(int x,int y,int w,int h);

public:
	aGfxDisplay();
	~aGfxDisplay();

	static aGfxDisplay *getActiveDisplay() { return activeDisplay; }

	int screenX() { return screen.x; }
	int screenY() { return screen.y; }
	int screenW() { return screen.w; }
	int screenH() { return screen.h; }

	virtual void start() { setActiveDisplay(this);setKeyFocus(this); }
	virtual void stop() {}

	virtual bool actionPerformed(aGfxActionEvent &ae) { return false; }
};



#endif /* _AMANITA_GFX_DISPLAY_H */

