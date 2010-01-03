
#include "../config.h"
#include <stdio.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aDisplay.h>


aObject_Inheritance(aDisplay,aComponent);


aDisplay *aDisplay::activeDisplay = 0;

aDisplay::aDisplay() : aComponent(0,0,g.getScreenWidth(),g.getScreenHeight()) {
	// setInstance(Screen::aClass);
	setScreen(0,0,getWidth(),getHeight());
	setKeyListener(this);
	setMouseListener(this);
	setMouseMotionListener(this);
}

aDisplay::~aDisplay() {
}

void aDisplay::setScreen(int x,int y,int w,int h) {
	screen.x = x;
	screen.y = y;
	screen.w = w;
	screen.h = h;
}


