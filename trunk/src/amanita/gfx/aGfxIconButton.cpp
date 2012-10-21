
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/gfx/aGfxImage.h>
#include <amanita/gfx/aGfxIconButton.h>


Object_Inheritance(aGfxIconButton,aGfxComponent)


extern char indent[32];


aGfxIconButton::_settings aGfxIconButton::_s = { 0 };

aGfxIconButton::aGfxIconButton(int id,int x,int y) : aGfxComponent(x,y) {
	setID(id);
	setImage(_s.img,_s.plain,_s.active,_s.down,_s.disabled);
	setMouseListener(this);
}
aGfxIconButton::aGfxIconButton(aGfxIconButton &b) : aGfxComponent(b),ib(b.ib) {
	setMouseListener(this);
}
aGfxIconButton::~aGfxIconButton() {
printf("%sIconButton::~aGfxIconButton()\n",indent);
}

void aGfxIconButton::setDefaultSettings(aGfxImage *img,uint32_t data[4]) {
	_s = (_settings){
		img,
		data[0],data[1],data[2],data[3],
	};
}

void aGfxIconButton::setImage(aGfxImage *img,int plain,int active,int down,int disabled) {
	ib = (iconbutton){ img,plain,active,down,disabled };
	if(ib.img) {
		SDL_Rect *r = ib.img->getCell(ib.plain);
		setSize(r->w,r->h);
	}
}

void aGfxIconButton::paint(time_t time) {
	if(ib.img) ib.img->draw(getX(),getY(),
		isEnabled()? (isMouseOver()? (isMouseDown()? ib.down : ib.active) : ib.plain) : ib.disabled);
}

bool aGfxIconButton::mouseUp(aGfxMouseEvent &me) {
printf("aGfxIconButton(%p)::mouseUp(source=%p,mouseListener=%p,actionListener=%p)\n",this,me.source,mouseListener,actionListener);
	if(isVisible() && isEnabled() && contains(me.x,me.y) && actionListener) {
		aGfxActionEvent ae = { me.source,me.button };
		actionListener->actionPerformed(ae);
		return true;
	}
	return false;
}

