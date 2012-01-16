
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <amanita/sdl/aGraphics.h>
#include <amanita/sdl/aComponent.h>
#include <amanita/sdl/aTooltip.h>
#include <amanita/sdl/aDisplay.h>


aObject_Inheritance(aComponent,aObject);


aComponent *aComponent::componentFocus = 0;
aComponent *aComponent::keyFocus = 0;
aComponent *aComponent::mouseDownFocus = 0;
aComponent *aComponent::mouseOverFocus = 0;
aComponent *aComponent::toolTipFocus = 0;

bool aComponent::setaComponentFocus(aComponent *c) {
	if(c!=componentFocus) {
		bool ret = true;
		if(componentFocus && componentFocus->focusListener) {
			aFocusEvent fe = { c,componentFocus };
			ret = componentFocus->focusListener->focusLost(fe);
		}
		if(ret && c && c->focusListener) {
			aFocusEvent fe = { c,componentFocus };
			ret = c->focusListener->focusGained(fe);
			componentFocus = c;
		}
		return ret;
	}
	return true;
}
bool aComponent::setKeyFocus(aComponent *c) {
	if(c!=keyFocus || !c->isVisible()) {
		while(c && (!c->keyListener || !c->isVisible())) c = c->com.p;
		if(c!=keyFocus) {
			if(!setaComponentFocus(c = c && c->keyListener && c->isVisible()? c : aDisplay::getActiveDisplay()))
				return false;
			keyFocus = c;
		}
	}
	return true;
}

aComponent::aComponent(int x,int y,int w,int h) : aObject() {
	com = (component){ 0ul,0,ENABLED|VISIBLE|LOCKED|OPAQUE,x,y,w,h,0,0,0 };
	if(!keyFocus) keyFocus = this;
	focusListener = 0,keyListener = 0,mouseListener = 0,mouseMotionListener = 0,actionListener = 0;
}
aComponent::aComponent(aComponent &c) : aObject(),com(c.com) {
	com.cnt = 0ul,com.p = 0,com.ch = 0,com.tt = 0;
	if(!keyFocus) keyFocus = this;
	focusListener = c.focusListener,keyListener = c.keyListener,mouseListener = c.mouseListener;
	mouseMotionListener = c.mouseMotionListener,actionListener = c.actionListener;
}

char indent[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int indentn = 0;

aComponent::~aComponent() {
printf("%saComponent::~aComponent(parent=%p,%" PRIx32 ",instance=%p,%" PRIx32 ",x=%d,y=%d,w=%d,h=%d,size=%lu)\n",
indent,com.p,com.p? ((aClass &)*com.p).getID() : 0,this,((aClass &)*this).getID(),com.x,com.y,com.w,com.h,(unsigned long)(com.ch? com.ch->size() : 0));
indent[indentn++] = '\t';
	if(com.ch) {
		aComponent *c;
		for(long i=0,n=com.ch->size(); i<n; i++) {
			c = (aComponent *)(*com.ch)[i];
			if(--c->com.cnt==0) delete c;
		}
		delete com.ch;
		com.ch = 0;
	}
	if(com.tt && (--com.tt->tt.cnt)==0) { delete com.tt;com.tt = 0; }
indent[--indentn] = 0;
printf("%saComponent::~aComponent()\n",indent);
}

void aComponent::add(aComponent *c) {
	if(!c) return;
	if(c->com.p) c->com.p->remove(c);
	if(!com.ch) com.ch = new aVector();
	*com.ch += c;
	c->com.cnt++;
	c->com.p = this;
	c->moveLocation(com.x,com.y);
}

void aComponent::remove(aComponent *c) {
	if(!c || !com.ch || !com.ch->contains(c)) return;
	*com.ch -= c;
	c->com.cnt--;
	c->com.p = 0;
	c->moveLocation(-com.x,-com.y);
	if(com.ch->size()==0) { delete com.ch;com.ch = 0; }
}

void aComponent::moveToTop() {
	if(com.p) {
		*com.p->com.ch -= this;
		*com.p->com.ch += this;
	}
}

aComponent *aComponent::getComponent(size_t i) {
	return com.ch && i<com.ch->size()? (aComponent *)(*com.ch)[i] : 0;
}

aComponent *aComponent::getComponent(int x,int y) {
	if(!isVisible()) return 0;
	aComponent *c = 0;
	if(com.ch) for(long i=com.ch->size()-1; !c && i>=0l; i--)
		c = ((aComponent *)(*com.ch)[i])->getComponent(x,y);
	return c? c :  (contains(x,y)? this : 0);
}

void aComponent::moveLocation(int x,int y) {
	com.x += x,com.y += y;
	if(com.ch) {
		aComponent *c;
		for(long i=com.ch->size()-1; i>=0l; i--) {
			c = (aComponent *)(*com.ch)[i];
			c->moveLocation(x,y);
		}
	}
}

bool aComponent::contains(aComponent *c) {
	while(c && c!=this) c = c->com.p;
	return c && c==this;
}
bool aComponent::contains(const SDL_Rect &r,int x,int y) {
	return x>=com.x+r.x && y>=com.y+r.y && x<com.x+r.x+r.w && y<com.y+r.y+r.h;
}

bool aComponent::isShowing() {
	return com.x+com.w>0 && com.y+com.h>0 &&
		com.x<g.getScreenWidth() && com.y<g.getScreenHeight();
}

void aComponent::paint(time_t time) {}

void aComponent::paintAll(time_t time) {
	if(!isVisible() || !com.ch) return;
	aComponent *c;
	if(clipBounds()) {
		SDL_Rect r1 = g.getClip(),r2 = { com.x,com.y,com.w,com.h };
		if(r2.x<r1.x) r2.w -= r1.x-r2.x,r2.x = r1.x;
		if(r2.x+r2.w>r1.x+r1.w) r2.w = r1.x+r1.w-r2.x;
		if(r2.y<r1.y) r2.h -= r1.y-r2.y,r2.y = r1.y;
		if(r2.y+r2.h>r1.y+r1.h) r2.h = r1.y+r1.h-r2.y;
		g.setClip(r2);
		for(size_t i=0ul; i<com.ch->size(); i++) if((c=(aComponent *)(*com.ch)[i])->isVisible()) {
			c->paint(time);
			c->paintAll(time);
		}
		g.setClip(r1);
	} else for(size_t i=0ul; i<com.ch->size(); i++) if((c=(aComponent *)(*com.ch)[i])->isVisible()) {
		c->paint(time);
		c->paintAll(time);
	}
}

short dragX = 0;
short dragY = 0;
int toolTipTimer = 0;

bool aComponent::handleKeyDown(aKeyEvent &ke) {
	if(keyFocus && keyFocus->keyListener) {
		ke.source = keyFocus;
		return keyFocus->keyListener->keyDown(ke);
	} else return false;
}
bool aComponent::handleKeyUp(aKeyEvent &ke) {
	if(keyFocus && keyFocus->keyListener) {
		ke.source = keyFocus;
		return keyFocus->keyListener->keyUp(ke);
	} else return false;
}
bool aComponent::handleMouseDown(aMouseEvent &me) {
	if(toolTipFocus) toolTipTimer = 0;
	if(!isVisible()) return false;
	aComponent *c = getComponent(me.x,me.y);
	if(!setKeyFocus(c)) return false;
	setMouseDownFocus(c);
	if(c) {
		me.source = c;
		if(!c->isLocked()) dragX = c->getX()-me.x,dragY = c->getY()-me.y;
		if(c->mouseListener) return c->mouseListener->mouseDown(me);
	}
	return false;
}
bool aComponent::handleMouseUp(aMouseEvent &me) {
	if(!isVisible()) return false;
	if(mouseDownFocus) {
		aComponent *c = mouseDownFocus;
		setMouseDownFocus(0);
		if(!contains(me.x,me.y)) setMouseOverFocus(0);
		if(c->mouseListener) {
			me.source = c;
			return c->mouseListener->mouseUp(me);
		}
	}
	return false;
}
bool aComponent::handleMouseMove(aMouseMotionEvent &mme) {
	if(!isVisible()) return false;
	aComponent *c = getComponent(mme.x,mme.y);
	setMouseOverFocus(c);
	if(c && c->mouseMotionListener) {
		mme.source = c;
		return c->mouseMotionListener->mouseMove(mme);
	}
	return false;
}
bool aComponent::handleMouseDrag(aMouseMotionEvent &mme) {
	if(isVisible() && mouseDownFocus) {
		aComponent *c = mouseDownFocus;
		mme.source = c;
		if(!c->isLocked()) c->setLocation(dragX+mme.x,dragY+mme.y);
		if(c->mouseMotionListener) return c->mouseMotionListener->mouseDrag(mme);
	}
	return false;
}


void aComponent::setToolTip(aTooltip *tt) {
	if(com.tt) com.tt->tt.cnt--;
	if(tt) tt->tt.cnt++;
	com.tt = tt;
	if(toolTipFocus==this) toolTipTimer = 0;
}

bool aComponent::handleToolTip(aTooltipEvent &tte) {
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

