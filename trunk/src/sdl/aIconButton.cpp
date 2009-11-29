
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libamanita/sdl/Image.h>
#include <libamanita/gui/IconButton.h>


RttiObjectInheritance(IconButton,Component);


extern char indent[32];


IconButton::_settings IconButton::_s = { 0 };

IconButton::IconButton(int id,int x,int y) : Component(x,y) {
	setID(id);
	setImage(_s.img,_s.plain,_s.active,_s.down,_s.disabled);
	setMouseListener(this);
}
IconButton::IconButton(IconButton &b) : Component(b),ib(b.ib) {
	setMouseListener(this);
}
IconButton::~IconButton() {
printf("%sIconButton::~IconButton()\n",indent);
}

void IconButton::setDefaultSettings(Image *img,Uint32 data[4]) {
	_s = (_settings){
		img,
		data[0],data[1],data[2],data[3],
	};
}

void IconButton::setImage(Image *img,int plain,int active,int down,int disabled) {
	ib = (iconbutton){ img,plain,active,down,disabled };
	if(ib.img) {
		SDL_Rect *r = ib.img->getCell(ib.plain);
		setSize(r->w,r->h);
	}
}

void IconButton::paint(time_t time) {
	if(ib.img) ib.img->draw(getX(),getY(),
		isEnabled()? (isMouseOver()? (isMouseDown()? ib.down : ib.active) : ib.plain) : ib.disabled);
}

bool IconButton::mouseUp(MouseEvent &me) {
printf("IconButton(%p)::mouseUp(source=%p,mouseListener=%p,actionListener=%p)\n",this,me.source,mouseListener,actionListener);
	if(isVisible() && isEnabled() && contains(me.x,me.y) && actionListener) {
		ActionEvent ae = { me.source,me.button };
		actionListener->actionPerformed(ae);
		return true;
	}
	return false;
}

