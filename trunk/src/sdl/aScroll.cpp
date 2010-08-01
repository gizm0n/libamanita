
#include "../config.h"
#include <amanita/sdl/aImage.h>
#include <amanita/sdl/aScroll.h>
#include <amanita/sdl/aIconButton.h>



aObject_Inheritance(aScroll,aComponent);


aScroll::_settings aScroll::_s = { 0 };

aScroll::aScroll(int id,int x,int y,int w,int h,int st) : aComponent(x,y,w,h) {
	// aObject::setInstance(aScroll::aClass);
	setID(id);
	val = min = 0,max = 100,inc = 10;
	bmin = new aIconButton(0);
	bmin->setActionListener(this);
	add(bmin);
	bmax = new aIconButton(1);
	bmax->setActionListener(this);
	add(bmax);
	bslide = new aIconButton(2);
	bslide->setImage(_s.img,_s.slide.plain,_s.slide.active,_s.slide.down,_s.slide.disabled);
	bslide->setLocked(false);
	bslide->setActionListener(this);
	bslide->setMouseMotionListener(this);
	add(bslide);
	setStyle(st);
	ae = (aActionEvent){ this,0 };
}

aScroll::~aScroll() {}

void aScroll::setDefaultSettings(aImage *img,uint32_t data[22]) {
	_s = (_settings){
		img,
		{ img->getCell(data[0]),data[0],data[1],data[2],data[3] },
		{ img->getCell(data[4]),data[4],data[5],data[6],data[7] },
		{ img->getCell(data[8]),data[8],data[9],data[10],data[11] },
		{ img->getCell(data[12]),data[12],data[13],data[14],data[15] },
		{ img->getCell(data[16]),data[16],data[17],data[18],data[19] },
		img->getCell(data[20]),img->getCell(data[21])
	};
}

void aScroll::setStyle(int st) {
	style = st;
	if(style&SCROLL_HORIZ) {
		bmin->setImage(_s.img,_s.left.plain,_s.left.active,_s.left.down,_s.left.disabled);
		bmin->setLocation(getX(),getY()+(style&SCROLL_TOP? 0 : getHeight()-bmin->getHeight()));
		bmax->setImage(_s.img,_s.right.plain,_s.right.active,_s.right.down,_s.right.disabled);
		bmax->setLocation(getX()+getWidth()-bmax->getWidth(),getY()+(style&SCROLL_TOP? 0 : getHeight()-bmax->getHeight()));
		bar = _s.hbar;
		slide.x = bmin->getWidth();
		if(style&SCROLL_TOP) slide.y = 0;
		else slide.y = getHeight()-bslide->getHeight();
		slide.w = getWidth()-bmin->getWidth()-bmax->getWidth();
		slide.h = _s.hbar->h;
	} else {
		bmin->setImage(_s.img,_s.up.plain,_s.up.active,_s.up.down,_s.up.disabled);
		bmin->setLocation(getX()+(style&SCROLL_LEFT? 0 : getWidth()-bmin->getWidth()),getY());
		bmax->setImage(_s.img,_s.down.plain,_s.down.active,_s.down.down,_s.down.disabled);
		bmax->setLocation(getX()+(style&SCROLL_LEFT? 0 : getWidth()-bmax->getWidth()),getY()+getHeight()-bmax->getHeight());
		bar = _s.vbar;
		if(style&SCROLL_LEFT) slide.x = 0;
		else slide.x = getWidth()-bslide->getWidth();
		slide.y = bmin->getHeight();
		slide.w = _s.vbar->w;
		slide.h = getHeight()-bmin->getHeight()-bmax->getHeight();
	}
	positionSlide();
}
void aScroll::setValues(int v,int mn,int mx,int i) {
	min = mn,max = mx,inc = i,val = v<min? min : (v>max? max : v);
	positionSlide();
	if(actionListener) actionListener->actionPerformed(ae);
}
void aScroll::setValue(int v) {
	val = v<min? min : (v>max? max : v);
	positionSlide();
	if(actionListener) actionListener->actionPerformed(ae);
}

void aScroll::positionSlide() {
	int x,y;
	if(style&SCROLL_HORIZ) {
		x = getX()+bmin->getWidth()+((val-min)*slide.w)/max,y = getY()+slide.y;
		if(x<getX()+slide.x) x = getX()+slide.x;
		else if(x>getX()+slide.x+slide.w-bslide->getWidth())
			x = getX()+slide.x+slide.w-bslide->getWidth();
	} else {
		x = getX()+slide.x,y = getY()+bmin->getHeight()+((val-min)*slide.h)/max;
		if(y<getY()+slide.y) y = getY()+slide.y;
		else if(y>=getY()+slide.y+slide.h-bslide->getHeight())
			y = getY()+slide.y+slide.h-bslide->getHeight();
	}
	bslide->setLocation(x,y);
}

bool aScroll::mouseDrag(aMouseMotionEvent &mme) {
	if(mme.source==bslide) {
		int v,x,y;
		if(style&SCROLL_HORIZ) {
			x = bslide->getX(),y = getY()+slide.y;
			if(x<getX()+slide.x) x = getX()+slide.x;
			else if(x>getX()+slide.x+slide.w-bslide->getWidth())
				x = getX()+slide.x+slide.w-bslide->getWidth();
		} else {
			x = getX()+slide.x,y = bslide->getY();
			if(y<getY()+slide.y) y = getY()+slide.y;
			else if(y>=getY()+slide.y+slide.h-bslide->getHeight())
				y = getY()+slide.y+slide.h-bslide->getHeight();
		}
		bslide->setLocation(x,y);

		if(style&SCROLL_HORIZ)
			v = min+((bslide->getX()-(getX()+slide.x))*(max-min))/(slide.w-bslide->getWidth());
		else v = min+((bslide->getY()-(getY()+slide.y))*(max-min))/(slide.h-bslide->getHeight());
		if(v<min) v = min;
		else if(v>max) v = max;

		if(v!=val) {
			val = v;
			if(actionListener) actionListener->actionPerformed(ae);
		}
		return true;
	}
	return false;
}

bool aScroll::actionPerformed(aActionEvent &ae) {
	if(ae.source==bmin || ae.source==bmax) {
		int v = val;
		if(ae.source==bmin) {
			v -= inc;
			if(v<min) v = min;
		} else {
			v += inc;
			if(v>max) v = max;
		}
		if(v!=val) {
			val = v;
			positionSlide();
			if(actionListener) actionListener->actionPerformed(this->ae);
		}
		return true;
	}
	return false;
}

void aScroll::paint(time_t time) {
	_s.img->draw(getX()+slide.x,getY()+slide.y,slide.w,slide.h,*bar);
}


