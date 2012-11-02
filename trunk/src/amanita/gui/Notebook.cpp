
#include "../_config.h"
#include <amanita/gui/Image.h>
#include <amanita/gui/Font.h>
#include <amanita/gui/Notebook.h>


namespace a {
namespace gui {

Object_Inheritance(Notebook,Widget)


Notebook::_settings Notebook::_s = { 0 };

Notebook::Notebook(int id,int x,int y,int st) : Widget(x,y,0,0),tabs(0) {
	setID(id);
	setStyle(st);
	ntabs = 0,act = 0;
	ae = (ActionEvent){ this,0 };
	setMouseListener(this);
}

Notebook::~Notebook() {
	deleteTabs();
}

void Notebook::setDefaultSettings(Image *img,Font *f1,Font *f2,uint32_t data[26]) {
	_s = (_settings){
		img,f1,f2,
		{
			img->getCell(data[0]),img->getCell(data[1]),img->getCell(data[2]),
			img->getCell(data[3]),img->getCell(data[4]),img->getCell(data[5]),
			img->getCell(data[6]),img->getCell(data[7]),img->getCell(data[8]),
			{ data[9],data[10],data[11],data[12], }
		},{
			img->getCell(data[13]),img->getCell(data[14]),img->getCell(data[15]),
			img->getCell(data[16]),img->getCell(data[17]),img->getCell(data[18]),
			img->getCell(data[19]),img->getCell(data[20]),img->getCell(data[21]),
			{ data[22],data[23],data[24],data[25], }
		}
	};
}

void Notebook::setStyle(int st) {
	style = st;
	t = style&TABSET_BOTTOM? &_s.bottom : &_s.top;
}

void Notebook::setTabs(const char *str[],int len) {
	if(!str || !len) return;
	deleteTabs();
	act = 0;
	ntabs = len;
	tabs = (tab *)malloc(sizeof(tab)*ntabs);
	for(int i=0; i<ntabs; i++) {
		tab &tb = tabs[i];
		tb.text = strdup(str[i]);
		tb.index = i;
		tb.tw1 = _s.font1->stringWidth(tb.text);
		tb.tw2 = _s.font2->stringWidth(tb.text);
		tb.r = (rect16_t){ 0,0,0,0 };
	}
	updateTabs();
}

void Notebook::updateTabs() {
	if(!tabs) return;
	rect16_t *r;
	int i,tw,w = 0,h = t->b2->h>t->b1->h? t->b2->h : t->b1->h;
	for(i=0; i<ntabs; i++) {
		tab &tb = tabs[i];
		if(i==0) w += i==act? t->l2->w : t->l1->w;
		tw = t->ins.x+(i==act? tb.tw2 : tb.tw1)+t->ins.w;
		r = i==ntabs-1? (i==act? t->r2 : t->r1) : (i==act? t->c3 : (i+1==act? t->c2 : t->c1));
		tb.r.x = w,tb.r.y = 0,tb.r.w = tw,tb.r.h = i==act? t->b2->h : t->b1->h;
		w += tw+r->w;
	}
	if(w!=getWidth() || h!=getHeight()) setSize(w,h);
}

void Notebook::deleteTabs() {
	if(!tabs) return;
	for(int i=0; i<ntabs; i++) free(tabs[i].text);
	free(tabs);
	tabs = 0,ntabs = 0,act = 0;
}

void Notebook::setActiveTab(int a) {
	if(a==act || !tabs) return;
	a = a<0? 0 : (a>=ntabs? ntabs-1 : a);
	int n = tabs[a].index;
	if(n>1) for(int i=0; i<ntabs; i++) if(i!=a && i!=act && tabs[i].index<n) tabs[i].index++;
	tabs[a].index = 0,tabs[act].index = 1,act = a;
	updateTabs();
}
int Notebook::getPreviousTab(int n) {
	if(tabs) {
      if(n<0 || n>=ntabs-1) return act;
      for(int i=0; i<ntabs; i++) if(tabs[i].index==n+1) return i;
	}
	return 0;
}

bool Notebook::mouseDown(MouseEvent &me) {
	if(tabs) for(int i=0; i<ntabs; i++) {
		rect16_t &r = tabs[i].r;
		if(me.x>=getX()+r.x && me.y>=getY()+r.y && me.x<getX()+r.x+r.w && me.y<getY()+r.y+r.h) {
			if(i!=act) {
				setActiveTab(i);
				ae.params = me.button;
				if(actionListener) actionListener->actionPerformed(ae);
			}
			return true;
		}
	}
	return false;
}

void Notebook::paint(time_t time) {
	if(!tabs) return;
	rect16_t *r;
	int i,x = 0,w;
	for(i=0; i<ntabs; i++) {
		tab &tb = tabs[i];
		if(i==0) {
			_s.img->draw(getX()+x,getY(),i==act? t->l2 : t->l1);
			x += i==act? t->l2->w : t->l1->w;
		}
		w = t->ins.x+(i==act? tb.tw2 : tb.tw1)+t->ins.w;
		r = i==ntabs-1? (i==act? t->r2 : t->r1) : (i==act? t->c3 : (i+1==act? t->c2 : t->c1));
		_s.img->draw(getX()+x,getY(),w,i==act? t->b2->h : t->b1->h,i==act? t->b2 : t->b1);
		_s.img->draw(getX()+x+w,getY(),r);
		if(style&TABSET_BOTTOM) {
			if(i==act) _s.font2->print(getX()+x+t->ins.x,getY()+t->b2->h-t->ins.h+_s.font2->getDescent(),tb.text);
			else _s.font1->print(getX()+x+t->ins.x,getY()+t->b1->h-t->ins.h+_s.font1->getDescent(),tb.text);
		} else {
			if(i==act) _s.font2->print(getX()+x+t->ins.x,getY()+t->ins.y+_s.font2->getAscent(),tb.text);
			else _s.font1->print(getX()+x+t->ins.x,getY()+t->ins.y+_s.font1->getAscent(),tb.text);
		}
		x += w+r->w;
	}
}


}; /* namespace gui */
}; /* namespace a */

