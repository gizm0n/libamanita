
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <libamanita/sdl/Graphics.h>
#include <libamanita/gui/Component.h>
#include <libamanita/gui/ToolTip.h>
#include <libamanita/gui/Display.h>


RttiObjectInheritance(Component,Object);


Component *Component::componentFocus = 0;
Component *Component::keyFocus = 0;
Component *Component::mouseDownFocus = 0;
Component *Component::mouseOverFocus = 0;
Component *Component::toolTipFocus = 0;

bool Component::setComponentFocus(Component *c) {
	if(c!=componentFocus) {
		bool ret = true;
		if(componentFocus && componentFocus->focusListener) {
			FocusEvent fe = { c,componentFocus };
			ret = componentFocus->focusListener->focusLost(fe);
		}
		if(ret && c && c->focusListener) {
			FocusEvent fe = { c,componentFocus };
			ret = c->focusListener->focusGained(fe);
			componentFocus = c;
		}
		return ret;
	}
	return true;
}
bool Component::setKeyFocus(Component *c) {
	if(c!=keyFocus || !c->isVisible()) {
		while(c && (!c->keyListener || !c->isVisible())) c = c->com.p;
		if(c!=keyFocus) {
			if(!setComponentFocus(c = c && c->keyListener && c->isVisible()? c : Display::getActiveDisplay()))
				return false;
			keyFocus = c;
		}
	}
	return true;
}

Component::Component(int x,int y,int w,int h) : Object() {
	com = (component){ 0ul,0,ENABLED|VISIBLE|LOCKED|OPAQUE,x,y,w,h,0,0,0 };
	if(!keyFocus) keyFocus = this;
	focusListener = 0,keyListener = 0,mouseListener = 0,mouseMotionListener = 0,actionListener = 0;
}
Component::Component(Component &c) : Object(),com(c.com) {
	com.cnt = 0ul,com.p = 0,com.ch = 0,com.tt = 0;
	if(!keyFocus) keyFocus = this;
	focusListener = c.focusListener,keyListener = c.keyListener,mouseListener = c.mouseListener;
	mouseMotionListener = c.mouseMotionListener,actionListener = c.actionListener;
}

char indent[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int indentn = 0;

Component::~Component() {
printf("%sComponent::~Component(parent=%p,%lx,instance=%p,%lx,x=%d,y=%d,w=%d,h=%d,size=%d)\n",
indent,com.p,com.p? ((Class &)*com.p).getID() : 0,this,((Class &)*this).getID(),com.x,com.y,com.w,com.h,com.ch? com.ch->size() : 0);
indent[indentn++] = '\t';
	if(com.ch) {
		Component *c;
		for(long i=0,n=com.ch->size(); i<n; i++) {
			c = (Component *)(*com.ch)[i];
			if(--c->com.cnt==0) delete c;
		}
		delete com.ch;
		com.ch = 0;
	}
	if(com.tt && (--com.tt->tt.cnt)==0) { delete com.tt;com.tt = 0; }
indent[--indentn] = 0;
printf("%sComponent::~Component()\n",indent);
}

void Component::add(Component *c) {
	if(!c) return;
	if(c->com.p) c->com.p->remove(c);
	if(!com.ch) com.ch = new Vector();
	*com.ch += c;
	c->com.cnt++;
	c->com.p = this;
	c->moveLocation(com.x,com.y);
}

void Component::remove(Component *c) {
	if(!c || !com.ch || !com.ch->contains(c)) return;
	*com.ch -= c;
	c->com.cnt--;
	c->com.p = 0;
	c->moveLocation(-com.x,-com.y);
	if(com.ch->size()==0) { delete com.ch;com.ch = 0; }
}

void Component::moveToTop() {
	if(com.p) {
		*com.p->com.ch -= this;
		*com.p->com.ch += this;
	}
}

Component *Component::getComponent(size_t i) {
	return com.ch && i<com.ch->size()? (Component *)(*com.ch)[i] : 0;
}

Component *Component::getComponent(int x,int y) {
	if(!isVisible()) return 0;
	Component *c = 0;
	if(com.ch) for(long i=com.ch->size()-1; !c && i>=0l; i--)
		c = ((Component *)(*com.ch)[i])->getComponent(x,y);
	return c? c :  (contains(x,y)? this : 0);
}

void Component::moveLocation(int x,int y) {
	com.x += x,com.y += y;
	if(com.ch) {
		Component *c;
		for(long i=com.ch->size()-1; i>=0l; i--) {
			c = (Component *)(*com.ch)[i];
			c->moveLocation(x,y);
		}
	}
}

bool Component::contains(Component *c) {
	while(c && c!=this) c = c->com.p;
	return c && c==this;
}
bool Component::contains(const SDL_Rect &r,int x,int y) {
	return x>=com.x+r.x && y>=com.y+r.y && x<com.x+r.x+r.w && y<com.y+r.y+r.h;
}

bool Component::isShowing() {
	return com.x+com.w>0 && com.y+com.h>0 &&
		com.x<g.getScreenWidth() && com.y<g.getScreenHeight();
}

void Component::paint(int time) {}

void Component::paintAll(int time) {
	if(!isVisible() || !com.ch) return;
	Component *c;
	if(clipBounds()) {
		SDL_Rect r1 = g.getClip(),r2 = { com.x,com.y,com.w,com.h };
		if(r2.x<r1.x) r2.w -= r1.x-r2.x,r2.x = r1.x;
		if(r2.x+r2.w>r1.x+r1.w) r2.w = r1.x+r1.w-r2.x;
		if(r2.y<r1.y) r2.h -= r1.y-r2.y,r2.y = r1.y;
		if(r2.y+r2.h>r1.y+r1.h) r2.h = r1.y+r1.h-r2.y;
		g.setClip(r2);
		for(size_t i=0ul; i<com.ch->size(); i++) if((c=(Component *)(*com.ch)[i])->isVisible()) {
			c->paint(time);
			c->paintAll(time);
		}
		g.setClip(r1);
	} else for(size_t i=0ul; i<com.ch->size(); i++) if((c=(Component *)(*com.ch)[i])->isVisible()) {
		c->paint(time);
		c->paintAll(time);
	}
}

short dragX = 0;
short dragY = 0;
int toolTipTimer = 0;

bool Component::handleKeyDown(KeyEvent &ke) {
	if(keyFocus && keyFocus->keyListener) {
		ke.source = keyFocus;
		return keyFocus->keyListener->keyDown(ke);
	} else return false;
}
bool Component::handleKeyUp(KeyEvent &ke) {
	if(keyFocus && keyFocus->keyListener) {
		ke.source = keyFocus;
		return keyFocus->keyListener->keyUp(ke);
	} else return false;
}
bool Component::handleMouseDown(MouseEvent &me) {
	if(toolTipFocus) toolTipTimer = 0;
	if(!isVisible()) return false;
	Component *c = getComponent(me.x,me.y);
	if(!setKeyFocus(c)) return false;
	setMouseDownFocus(c);
	if(c) {
		me.source = c;
		if(!c->isLocked()) dragX = c->getX()-me.x,dragY = c->getY()-me.y;
		if(c->mouseListener) return c->mouseListener->mouseDown(me);
	}
	return false;
}
bool Component::handleMouseUp(MouseEvent &me) {
	if(!isVisible()) return false;
	if(mouseDownFocus) {
		Component *c = mouseDownFocus;
		setMouseDownFocus(0);
		if(!contains(me.x,me.y)) setMouseOverFocus(0);
		if(c->mouseListener) {
			me.source = c;
			return c->mouseListener->mouseUp(me);
		}
	}
	return false;
}
bool Component::handleMouseMove(MouseMotionEvent &mme) {
	if(!isVisible()) return false;
	Component *c = getComponent(mme.x,mme.y);
	setMouseOverFocus(c);
	if(c && c->mouseMotionListener) {
		mme.source = c;
		return c->mouseMotionListener->mouseMove(mme);
	}
	return false;
}
bool Component::handleMouseDrag(MouseMotionEvent &mme) {
	if(isVisible() && mouseDownFocus) {
		Component *c = mouseDownFocus;
		mme.source = c;
		if(!c->isLocked()) c->setLocation(dragX+mme.x,dragY+mme.y);
		if(c->mouseMotionListener) return c->mouseMotionListener->mouseDrag(mme);
	}
	return false;
}


void Component::setToolTip(ToolTip *tt) {
	if(com.tt) com.tt->tt.cnt--;
	if(tt) tt->tt.cnt++;
	com.tt = tt;
	if(toolTipFocus==this) toolTipTimer = 0;
}

bool Component::handleToolTip(ToolTipEvent &tte) {
	if(mouseOverFocus && mouseOverFocus->com.tt) {
		toolTipTimer++;
		if(toolTipTimer>tte.showTimer && toolTipTimer<=tte.hideTimer) {
			if(toolTipFocus!=mouseOverFocus) setToolTipFocus(mouseOverFocus);
			tte.source = toolTipFocus;
			if(toolTipFocus->com.tt->paintToolTip(tte)) return true;
			else toolTipTimer = 0;
		}
	} else toolTipTimer = 0;
	if(toolTipFocus) setToolTipFocus(0);
	return false;
}

