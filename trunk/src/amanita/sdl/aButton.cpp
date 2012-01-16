
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/sdl/aImage.h>
#include <amanita/sdl/aFont.h>
#include <amanita/sdl/aText.h>
#include <amanita/sdl/aButton.h>


aObject_Inheritance(aButton,aComponent);


extern char indent[32];


aButton::_settings aButton::_s = { 0,0,{} };

aButton::aButton(int id,int x,int y,int w,int h) : aComponent(x,y,w,h) {
	setID(id);
	bt = (button){
		{ 0,0 },
		0,ALIGN_CENTER|ALIGN_MIDDLE,
		0,0,0,
		_s.font,
		{ 0,0,0,0 },
	};
	setMouseListener(this);
}
aButton::aButton(aButton &b) : aComponent(b),bt(b.bt) {
	bt.font = b.bt.font;
	bt.text = 0;
	if(b.bt.text) setText(b.bt.text);
	setMouseListener(this);
}
aButton::~aButton() {
printf("%saButton::~aButton()\n",indent);
	if(bt.text) { free(bt.text);bt.text = 0; }
}

void aButton::setDefaultSettings(aImage *img,aFont *f,uint32_t data[36]) {
	_s = (_settings){
		img,f,
		{{
			img->getCell(data[0]),img->getCell(data[1]),img->getCell(data[2]),
			img->getCell(data[3]),img->getCell(data[4]),img->getCell(data[5]),
			img->getCell(data[6]),img->getCell(data[7]),img->getCell(data[8])
		},{
			img->getCell(data[9]),img->getCell(data[10]),img->getCell(data[11]),
			img->getCell(data[12]),img->getCell(data[13]),img->getCell(data[14]),
			img->getCell(data[15]),img->getCell(data[16]),img->getCell(data[17])
		},{
			img->getCell(data[18]),img->getCell(data[19]),img->getCell(data[20]),
			img->getCell(data[21]),img->getCell(data[22]),img->getCell(data[23]),
			img->getCell(data[24]),img->getCell(data[25]),img->getCell(data[26])
		},{
			img->getCell(data[27]),img->getCell(data[28]),img->getCell(data[29]),
			img->getCell(data[30]),img->getCell(data[31]),img->getCell(data[32]),
			img->getCell(data[33]),img->getCell(data[34]),img->getCell(data[35])
		}}
	};
}

void aButton::setText(const char *text) {
	if(!text || !*text) return;
	if(bt.text) free(bt.text);
	bt.text = strdup(text);
	setFont(bt.font);
}

void aButton::setFont(aFont *font) {
	bt.font = font;
	bt.tw = bt.text? bt.font->stringWidth(bt.text) : 0;
	setTextAlign(bt.align);
}

void aButton::setTextAlign(int align) {
	bt.align = align;
	if(bt.align&ALIGN_CENTER) bt.tx = (getWidth()-bt.tw)/2;
	else if(bt.align&ALIGN_RIGHT) bt.tx = getWidth()-bt.tw-bt.ins.w;
	else bt.tx = bt.ins.x;
	if(bt.align&ALIGN_MIDDLE) bt.ty = (getHeight()+bt.font->getHeight())/2+bt.font->getDescent();
	else if(bt.align&ALIGN_BOTTOM) bt.ty = getHeight()-bt.font->getHeight()-bt.ins.h;
	else bt.ty = bt.ins.y;
}

void aButton::setInsets(int l,int t,int r,int b) {
	bt.ins = (SDL_Rect){ l,t,r,b };
	setTextAlign(bt.align);
}

void aButton::paint(time_t time) {
	if(isOpaque() && _s.img) {
		_settings::state &st = _s.st[isEnabled()? (isMouseOver()? (isMouseDown()? 2 : 1) : 0) : 3];
		_s.img->draw(getX()+st.l->w,getY()+st.t->h,getWidth()-st.l->w-st.r->w,getHeight()-st.t->h-st.b->h,*st.c);
		_s.img->draw(getX()+st.l->w,getY(),getWidth()-st.l->w-st.r->w,st.t->h,*st.t);
		_s.img->draw(getX(),getY()+st.t->h,st.l->w,getHeight()-st.t->h-st.b->h,*st.l);
		_s.img->draw(getX()+getWidth()-st.r->w,getY()+st.t->h,st.r->w,getHeight()-st.t->h-st.b->h,*st.r);
		_s.img->draw(getX()+st.l->w,getY()+getHeight()-st.b->h,getWidth()-st.l->w-st.r->w,st.b->h,*st.b);

		_s.img->draw(getX(),getY(),*st.tl);
		_s.img->draw(getX()+getWidth()-st.tr->w,getY(),*st.tr);
		_s.img->draw(getX(),getY()+getHeight()-st.bl->h,*st.bl);
		_s.img->draw(getX()+getWidth()-st.br->w,getY()+getHeight()-st.br->h,*st.br);
	}
	if(bt.ic.img) {
		SDL_Rect &r = *bt.ic.img->getCell(bt.ic.index);
		bt.ic.img->draw(getX()+(getWidth()-r.w)/2,getY()+(getHeight()-r.h)/2,r);
	}
	if(bt.text) bt.font->print(getX()+bt.tx,getY()+bt.ty+(isEnabled() && isMouseDown()? 1 : 0),bt.text);
}

bool aButton::mouseUp(aMouseEvent &me) {
printf("aButton(%p)::mouseUp(source=%p,mouseListener=%p,actionListener=%p)\n",this,me.source,mouseListener,actionListener);
	if(isVisible() && isEnabled() && contains(me.x,me.y) && actionListener) {
		aActionEvent ae = { me.source,me.button };
		actionListener->actionPerformed(ae);
		return true;
	}
	return false;
}



