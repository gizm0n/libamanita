
#include "../_config.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <amanita/gui/Font.h>
#include <amanita/gui/Window.h>
#include <amanita/gui/Button.h>
#include <amanita/gui/Icon.h>
#include <amanita/gui/Tooltip.h>


namespace a {
namespace gui {

Object_Inheritance(Window,Widget)


extern char indent[32];


Window::_settings Window::_s = { 0 };


Window::Window(int x,int y,int w,int h,int st)
		: Widget(x,y,w,h),style(0),bgimg(0),cb(0),tcap(0),bcap(0),tf(0),bf(0) {
	Widget::setLocked(false);
	setFocusListener(this);
	setMouseListener(this);
	setStyle(st);
	bgimg = _s.bgimg,bgindex = _s.bgindex,bgcol = _s.bgcol;
}

Window::~Window() {
fprintf(stderr,"%sDialog::~Window()\n",indent);
}

void Window::setDefaultSettings(Image *img,Image *bgimg,Font *f,uint32_t data[21]) {
	_s = (_settings){
		img,bgimg,data[0],data[0],f,
		{
			img->getCell(data[1]),img->getCell(data[3]),img->getCell(data[5]),img->getCell(data[7]),
			img->getCell(data[8]),img->getCell(data[9]),img->getCell(data[11]),img->getCell(data[13])
		},{
			img->getCell(data[2]),img->getCell(data[4]),img->getCell(data[6]),img->getCell(data[7]),
			img->getCell(data[8]),img->getCell(data[10]),img->getCell(data[12]),img->getCell(data[14])
		},
		{ (int)data[15],(int)data[16],(int)data[17],(int)data[18],(int)data[19],(int)data[20] }
	};
}

void Window::setStyle(int st) {
	fr = _s.fr;
	if(st&CAPTION_TOP) fr.tl = _s.cap.tl,fr.t = _s.cap.t,fr.tr = _s.cap.tr;
	if(st&CAPTION_BOTTOM) fr.bl = _s.cap.bl,fr.b = _s.cap.b,fr.br = _s.cap.br;
	if(st&CLOSE_BUTTON) {
		if(!cb) {
			cb = new Icon(COM_ID_CLOSE);
			cb->setImage(_s.img,_s.cb.plain,_s.cb.active,_s.cb.down,_s.cb.disabled);
			cb->setLocation(getWidth()-_s.cb.x-cb->getWidth(),_s.cb.y);
			cb->setActionListener(this);
			cb->setToolTip(new Tooltip("Close"));
			add(cb);
fprintf(stderr,"Window::setDialogStyle(%p,instance=%" PRIx32 ",actionListener=%p)\n",cb,((Class &)*cb).getID(),cb->getActionListener());
fflush(stderr);
		}
	} else if(cb) {
		remove(cb);
		delete cb;
		cb = 0;
	}
	style = st;
}

void Window::moveToCenter() {
	setLocation((g.getWidth()-getWidth())/2,(g.getHeight()-getHeight())/2);
}

bool Window::mouseDown(MouseEvent &me) {
	moveToTop();
	return false;
}

bool Window::actionPerformed(ActionEvent &ae) {
	if(ae.source->getID()==COM_ID_CLOSE) { hide();return true; }
	return false;
}

void Window::paint(time_t time) {
	if(isOpaque()) {
		if(bgimg) bgimg->draw(getX()+fr.l->w,getY()+fr.t->h,getWidth()-fr.l->w-fr.r->w,getHeight()-fr.t->h-fr.b->h,bgindex);
		else g.fillRect(getX()+fr.l->w,getY()+fr.t->h,getWidth()-fr.l->w-fr.r->w,getHeight()-fr.t->h-fr.b->h,g.mapRGB(bgcol));
	}
	_s.img->draw(getX()+fr.l->w,getY(),getWidth()-fr.l->w-fr.r->w,fr.t->h,*fr.t);
	_s.img->draw(getX(),getY()+fr.t->h,fr.l->w,getHeight()-fr.t->h-fr.b->h,*fr.l);
	_s.img->draw(getX()+getWidth()-fr.r->w,getY()+fr.t->h,fr.r->w,getHeight()-fr.t->h-fr.b->h,*fr.r);
	_s.img->draw(getX()+fr.l->w,getY()+getHeight()-fr.b->h,getWidth()-fr.l->w-fr.r->w,fr.b->h,*fr.b);
	_s.img->draw(getX(),getY(),*fr.tl);
	_s.img->draw(getX()+getWidth()-fr.tr->w,getY(),*fr.tr);
	_s.img->draw(getX(),getY()+getHeight()-fr.bl->h,*fr.bl);
	_s.img->draw(getX()+getWidth()-fr.br->w,getY()+getHeight()-fr.br->h,*fr.br);
	if(tcap && tf) tf->print(getX()+fr.tl->w+1,getY()+fr.t->h+tf->getDescent()-1,tcap);
	if(bcap && bf) bf->print(getX()+fr.bl->w+1,getY()+getHeight()+bf->getDescent()-3,bcap);
}

}; /* namespace gui */
}; /* namespace a */



