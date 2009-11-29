
#include "../config.h"
#include <stdio.h>
#include <libamanita/sdl/Graphics.h>
#include <libamanita/sdl/Thread.h>
#include <libamanita/gui/Display.h>


RttiObjectInheritance(Display,Component);


Display *Display::activeDisplay = 0;

Display::Display() : Component(0,0,g.getScreenWidth(),g.getScreenHeight()) {
	// setInstance(Screen::Class);
	setScreen(0,0,getWidth(),getHeight());
	setKeyListener(this);
	setMouseListener(this);
	setMouseMotionListener(this);
}

Display::~Display() {
}

void Display::setScreen(int x,int y,int w,int h) {
	screen.x = x,screen.y = y,screen.w = w,screen.h = h;
}


