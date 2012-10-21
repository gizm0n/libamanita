
#include "../_config.h"
#include <stdio.h>
#include <amanita/gfx/aGfxGraphics.h>
#include <amanita/gfx/aGfxDisplay.h>


Object_Inheritance(aGfxDisplay,aGfxComponent)


aGfxDisplay *aGfxDisplay::activeDisplay = 0;

aGfxDisplay::aGfxDisplay() : aGfxComponent(0,0,g.getScreenWidth(),g.getScreenHeight()) {
	// setInstance(Screen::Class);
	setScreen(0,0,getWidth(),getHeight());
	setKeyListener(this);
	setMouseListener(this);
	setMouseMotionListener(this);
}

aGfxDisplay::~aGfxDisplay() {
}

void aGfxDisplay::setScreen(int x,int y,int w,int h) {
	screen.x = x;
	screen.y = y;
	screen.w = w;
	screen.h = h;
}


