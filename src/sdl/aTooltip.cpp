
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aImage.h>
#include <libamanita/sdl/aTooltip.h>


extern char indent[32];


aTooltip::_settings aTooltip::_s = { 0,0,0,0,0,0,0,0,0,0,0 };

aTooltip::aTooltip() {
	tt = (tooltip){ 0ul,0,0,0,-1,-1,0,0 };
}
aTooltip::aTooltip(const char *text) {
	tt = (tooltip){ 0ul,0,0,0,-1,-1,0,0 };
	if(text && *text) setText(text);
}
aTooltip::~aTooltip() {
	if(tt.text) { free(tt.text);tt.text = 0; }
	if(tt.lines) { free(tt.lines);tt.lines = 0,tt.nlines = 0; }
}

void aTooltip::setDefaultSettings(aImage *img,aFont *f,Uint32 data[13]) {
	_s = (_settings){
		img,f,
		img->getCell(data[0]),img->getCell(data[1]),img->getCell(data[2]),
		img->getCell(data[3]),img->getCell(data[4]),img->getCell(data[5]),
		img->getCell(data[6]),img->getCell(data[7]),img->getCell(data[8]),
		{ data[9],data[10],data[11],data[12] }
	};
}

void aTooltip::setText(const char *text) {
	char *p,**l;
	if(tt.text) free(tt.text);
	tt.text = strdup(text);
	for(tt.nlines=1,p=tt.text; *p; p++) if(*p=='\n') tt.nlines++;
	if(tt.nlines>1) {
		int w = 0;
		tt.lines = (char **)malloc(sizeof(char *)*tt.nlines);
		for(l=tt.lines,p=tt.text,*l++=p; *p; p++) if(*p=='\n') {
			*p++ = '\0',w = _s.font->stringWidth(*l);
			if(w>tt.w) tt.w = w;
			*l++ = p;
		}
	} else tt.lines = 0,tt.w = _s.l->w+_s.font->stringWidth(tt.text)+_s.r->w;
	tt.w += _s.ins.x+_s.ins.w;
	tt.h = _s.t->h+_s.font->getHeight()*tt.nlines+_s.b->h+_s.ins.y+_s.ins.h;
}

bool aTooltip::paintToolTip(aTooltipEvent &tte) {
	if(!tt.text || !*tt.text) return false;
	tt.x = tte.x,tt.y = tte.y+16;
	if(tt.x+tt.w>g.getScreenWidth()) tt.x = g.getScreenWidth()-1-tt.w;
	if(tt.y+tt.h>g.getScreenHeight()) tt.y = tte.y-1-tt.h;

	paintToolTipRect(tt.x,tt.y,tt.w,tt.h);
	int y = tt.y+_s.t->h+_s.ins.y+_s.font->getAscent();
	if(tt.lines) for(int i=0; i<tt.nlines; i++,y+=_s.font->getHeight())
		_s.font->print(tt.x+_s.l->w+_s.ins.x,y,tt.lines[i]);
	else _s.font->print(tt.x+_s.l->w+_s.ins.x,y,tt.text);
	return true;
}

void aTooltip::paintToolTipRect(int x,int y,int w,int h) {
	_s.img->draw(x+_s.l->w,y+_s.t->h,w-_s.l->w-_s.r->w,h-_s.t->h-_s.b->h,_s.c);
	_s.img->draw(x,y,_s.tl);
	_s.img->draw(x+_s.tl->w,y,w-_s.tl->w-_s.tr->w,_s.t->h,_s.t);
	_s.img->draw(x+w-_s.tr->w,y,_s.tr);
	_s.img->draw(x,y+_s.tl->h,_s.l->w,h-_s.tl->h-_s.bl->h,_s.l);
	_s.img->draw(x+w-_s.r->w,y+_s.tl->h,_s.r->w,h-_s.tl->h-_s.bl->h,_s.r);
	_s.img->draw(x,y+h-_s.bl->h,_s.bl);
	_s.img->draw(x+_s.bl->w,y+h-_s.b->h,w-_s.bl->w-_s.br->w,_s.b->h,_s.b);
	_s.img->draw(x+w-_s.tr->w,y+h-_s.bl->h,_s.br);
}

