
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/gui/Image.h>
#include <amanita/gui/Icon.h>


namespace a {
namespace gui {

Object_Inheritance(Icon,Widget)


extern char indent[32];


Icon::_settings Icon::_s = { 0 };

Icon::Icon(int id,int x,int y) : Widget(x,y) {
	setID(id);
	setImage(_s.img,_s.plain,_s.active,_s.down,_s.disabled);
	setMouseListener(this);
}
Icon::Icon(Icon &b) : Widget(b),ib(b.ib) {
	setMouseListener(this);
}
Icon::~Icon() {
printf("%sIconButton::~Icon()\n",indent);
}

void Icon::setDefaultSettings(Image *img,uint32_t data[4]) {
	_s = (_settings){
		img,
		(short)data[0],(short)data[1],(short)data[2],(short)data[3],
	};
}

void Icon::setImage(Image *img,int plain,int active,int down,int disabled) {
	ib = (iconbutton){ img,(short)plain,(short)active,(short)down,(short)disabled };
	if(ib.img) {
		rect16_t *r = ib.img->getCell(ib.plain);
		setSize(r->w,r->h);
	}
}

void Icon::paint(time_t time) {
	if(ib.img) ib.img->draw(getX(),getY(),
		isEnabled()? (isMouseOver()? (isMouseDown()? ib.down : ib.active) : ib.plain) : ib.disabled);
}

bool Icon::mouseUp(MouseEvent &me) {
printf("Icon(%p)::mouseUp(source=%p,mouseListener=%p,actionListener=%p)\n",this,me.source,mouseListener,actionListener);
	if(isVisible() && isEnabled() && contains(me.x,me.y) && actionListener) {
		ActionEvent ae = { me.source,me.button };
		actionListener->actionPerformed(ae);
		return true;
	}
	return false;
}

}; /* namespace gui */
}; /* namespace a */

