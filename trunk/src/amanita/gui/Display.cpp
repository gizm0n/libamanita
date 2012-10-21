
#include "../_config.h"
#include <stdio.h>
#include <amanita/gui/Graphics.h>
#include <amanita/gui/Display.h>


namespace a {
namespace gui {

Object_Inheritance(Display,Widget)


Display *Display::activeDisplay = 0;

Display::Display() : Widget(0,0,g.getScreenWidth(),g.getScreenHeight()) {
	// setInstance(Screen::Class);
	setScreen(0,0,getWidth(),getHeight());
	setKeyListener(this);
	setMouseListener(this);
	setMouseMotionListener(this);
}

Display::~Display() {
}

void Display::setScreen(int x,int y,int w,int h) {
	screen.x = x;
	screen.y = y;
	screen.w = w;
	screen.h = h;
}

}; /* namespace gui */
}; /* namespace a */


