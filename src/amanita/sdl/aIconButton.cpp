
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/sdl/aImage.h>
#include <amanita/sdl/aIconButton.h>


aObject_Inheritance(aIconButton,aComponent)


extern char indent[32];


aIconButton::_settings aIconButton::_s = { 0 };

aIconButton::aIconButton(int id,int x,int y) : aComponent(x,y) {
	setID(id);
	setImage(_s.img,_s.plain,_s.active,_s.down,_s.disabled);
	setMouseListener(this);
}
aIconButton::aIconButton(aIconButton &b) : aComponent(b),ib(b.ib) {
	setMouseListener(this);
}
aIconButton::~aIconButton() {
printf("%sIconButton::~aIconButton()\n",indent);
}

void aIconButton::setDefaultSettings(aImage *img,uint32_t data[4]) {
	_s = (_settings){
		img,
		data[0],data[1],data[2],data[3],
	};
}

void aIconButton::setImage(aImage *img,int plain,int active,int down,int disabled) {
	ib = (iconbutton){ img,plain,active,down,disabled };
	if(ib.img) {
		SDL_Rect *r = ib.img->getCell(ib.plain);
		setSize(r->w,r->h);
	}
}

void aIconButton::paint(time_t time) {
	if(ib.img) ib.img->draw(getX(),getY(),
		isEnabled()? (isMouseOver()? (isMouseDown()? ib.down : ib.active) : ib.plain) : ib.disabled);
}

bool aIconButton::mouseUp(aMouseEvent &me) {
printf("aIconButton(%p)::mouseUp(source=%p,mouseListener=%p,actionListener=%p)\n",this,me.source,mouseListener,actionListener);
	if(isVisible() && isEnabled() && contains(me.x,me.y) && actionListener) {
		aActionEvent ae = { me.source,me.button };
		actionListener->actionPerformed(ae);
		return true;
	}
	return false;
}

