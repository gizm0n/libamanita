
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/gui/aContainer.h>
#include <amanita/gui/aWindow.h>



aObject_Inheritance(aContainer,aWidget)

aContainer::aContainer() : aWidget(0,aWIDGET_CONTAINER) {
	child = 0;
	children = 0;
}

aContainer::aContainer(widget_event_handler weh,widget_type t) : aWidget(weh,t) {
	child = 0;
	children = 0;
}

aContainer::~aContainer() {
	if(child) delete child;
	child = 0;
	children = 0;
}

void aContainer::add(aWidget *w) {
	if(children==0xff) return; // Will only accept up to 255 children.
debug_output("aContainer::add(w=%p)\n",w);
	w->parent = this;
	if(!child) child = w;
	else {
		aWidget *c;
		for(c=child; c->next; c=c->next);
		c->next = w;
	}
	++children;
debug_output("aContainer::add(child=%p)\n",child);
}

aWidget *aContainer::remove(aWidget *w) {
	return 0;
}

aWidget *aContainer::remove(uint32_t id) {
	return 0;
}


void aContainer::create(aWindow *wnd,uint32_t st) {
	if(text) type = aWIDGET_FRAME;
	else if(parent && *parent==aContainer::getClass()) type = aWIDGET_VOID;
debug_output("aContainer::create(type: %d)\n",type);

#ifdef USE_GTK
	if((style&aHORIZONTAL)) {
		component = (aComponent)gtk_hbox_new(
			(style&aHOMOGENOUS)? TRUE : FALSE,spacing);
	} else if((style&aVERTICAL)) {
		component = (aComponent)gtk_vbox_new(
			(style&aHOMOGENOUS)? TRUE : FALSE,spacing);
	} else if((style&aTABLE)) {
	}
	if(border) gtk_container_set_border_width(GTK_CONTAINER(component),border);
#endif

	aWidget::create(wnd,0);

#ifdef USE_WIN32
	captureEvents();
#endif
}

void aContainer::createAll(aComponent p,bool n) {
	aWidget *w;
debug_output("aContainer::createAll(p: %p)\n",p);
#ifdef USE_GTK
	if(child) {
		for(w=child; w; w=w->next) {
			w->create(window,0);
			w->createAll(0,false);
			if((style&aHORIZONTAL) || (style&aVERTICAL)) {
				gtk_box_pack_start(GTK_BOX(component),(GtkWidget *)w->component,
					(w->style&aEXPAND)? TRUE : FALSE,
					(w->style&aFILL)? TRUE : FALSE,
					0);
			} else if((style&aTABLE)) {
			}
		}
		if(type==aWIDGET_FRAME) {
			GtkWidget *frame = gtk_frame_new(text);
			GtkWidget *vbox = gtk_vbox_new(TRUE,0);
			gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget *)component,TRUE,TRUE,0);
			gtk_container_set_border_width(GTK_CONTAINER(vbox),6);
			gtk_container_add(GTK_CONTAINER(frame),(GtkWidget *)vbox);
			component = frame;
		}
	}
	if(n && next) {
		for(w=next; w; w=w->next) {
			w->create(window,0);
			w->createAll(p,false);
		}
	}
	if(p) gtk_container_add(GTK_CONTAINER(p),(GtkWidget *)component);
#endif

#ifdef USE_WIN32
debug_output("aContainer::createAll()\n");
	if(n && next) {
		for(w=next; w; w=w->next) {
			w->create(window,0);
			w->createAll(p,false);
		}
	}
	if(child) {
		if(type==aWIDGET_VOID) {
			aWidget *wp = parent;
			while(wp && wp->type==aWIDGET_VOID) wp = wp->parent;
			for(w=child; w; w=w->next) {
				w->parent = wp;
				w->create(window,0);
				w->createAll(wp->component,false);
				w->parent = this;
			}
		} else {
			for(w=child; w; w=w->next) {
				w->create(window,0);
				w->createAll(component,false);
			}
		}
	}
#endif
}


#ifdef USE_WIN32
void aContainer::makeLayout(int x,int y,int w,int h) {
debug_output("aContainer::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",x,y,w,h);
	aWidget::makeLayout(x,y,w,h);
//	if(border) this->x += border,this->y += border,width -= border*2,height -= border*2;
	if(child) {
		aWidget *c = child;
		int i,n = 0,sz[children],min,max,sum,exp,nexp,sp = (children-1)*spacing;
		int x1 = 0,y1 = 0,w1 = 0,h1 = 0,w2 = width,h2 = height;
		if(type==aWIDGET_VOID) x1 += this->x,y1 += this->y;
		else if(type==aWIDGET_FRAME)
			x1 += MulDiv(dbu_x,7,4),y1 += MulDiv(dbu_y,11,8),w2 -= MulDiv(dbu_x,7+7,4),h1 -= MulDiv(dbu_y,11+7,8);
		if((style&aHORIZONTAL)) {
			for(i=0,c=child,min=0,max=0,sum=0,exp=0,nexp=0; c; ++i,c=c->next) {
				sum += sz[i] = n = c->getMinimumWidth();
				if(!min || min>n) min = n;
				if(max<n) max = n;
				if((c->style&aEXPAND)) ++exp;
				else nexp += n;
			}
debug_output("aContainer::makeLayout(min: %d, max: %d, exp: %d, nexp: %d)\n",min,max,exp,nexp);
			if((style&aHOMOGENOUS)) {
				for(w1=(w2-sp)/children,c=child; c; x1+=w1+spacing,c=c->next) {
					if(!c->next) w1 = w2-x1;
debug_output("aContainer::makeLayout(aHOMOGENOUS x1: %d, w1: %d  %s)\n",x1,w1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w1,h2);
				}
			} else {
				for(i=0,c=child; c; ++i,x1+=w1+spacing,c=c->next) {
					if((c->style&aEXPAND)) {
						if(!c->next) w1 = w2-x1;
						else w1 = (w2-sp-nexp)/exp;
					} else w1 = sz[i];
debug_output("aContainer::makeLayout(x1: %d, w1: %d  %s)\n",x1,w1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w1,h2);
				}
			}

		} else if((style&aVERTICAL)) {
			for(i=0,c=child,min=0,max=0,sum=0,exp=0,nexp=0; c; ++i,c=c->next) {
				sum += sz[i] = n = c->getMinimumHeight();
				if(!min || min>n) min = n;
				if(max<n) max = n;
				if((c->style&aEXPAND)) ++exp;
				else nexp += n;
			}
debug_output("aContainer::makeLayout(min: %d, max: %d, exp: %d, nexp: %d)\n",min,max,exp,nexp);
			if((style&aHOMOGENOUS)) {
				for(h1=(h2-sp)/children,c=child; c; y1+=n+spacing,c=c->next) {
					if(!c->next) h1 = h2-y1;
debug_output("aContainer::makeLayout(aHOMOGENOUS y1: %d, h1: %d  %s)\n",y1,h1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w2,h1);
				}
			} else {
				for(i=0,c=child; c; ++i,y1+=h1+spacing,c=c->next) {
					if((c->style&aEXPAND)) {
						if(!c->next) h1 = h2-y1;
						else h1 = (h2-sp-nexp)/exp;
					} else h1 = sz[i];
debug_output("aContainer::makeLayout(y1: %d, h1: %d  %s)\n",y1,h1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w2,h1);
				}
			}
		} else if((style&aTABLE)) {
			
		} else {
			for(; c; c=c->next)
				c->makeLayout(x,y,w,h);
		}
	}
}

int aContainer::getMinimumWidth() {
	if(!(style&aFIXED)) {
		aWidget *w;
		int box = 0;
		if(type==aWIDGET_FRAME) box = MulDiv(dbu_x,7+7,4);
		if((style&aHORIZONTAL)) {
			int n = border*2+(children-1)*spacing;
			for(w=child; w; w=w->next) n += w->getMinimumWidth();
			return box+n;
		} else if((style&aVERTICAL)) {
			int n,max = 0; // We want to know the largest item to know minimum width of container.
			for(w=child; w; w=w->next) {
				n = w->getMinimumWidth();
				if(n>max) max = n;
			}
			return box+max;
		}
	}
	return min_width+border*2;
}

int aContainer::getMinimumHeight() {
	if(!(style&aFIXED)) {
		aWidget *w;
		int box = 0;
		if(type==aWIDGET_FRAME) box = MulDiv(dbu_y,11+7,8);
		if((style&aVERTICAL)) {
			int n = border*2+(children-1)*spacing;
			for(w=child; w; w=w->next) n += w->getMinimumHeight();
			return box+n;
		} else if((style&aHORIZONTAL)) {
			int n,max = 0; // We want to know the largest item to know minimum width of container.
			for(w=child; w; w=w->next) {
				n = w->getMinimumHeight();
				if(n>max) max = n;
			}
			return box+max;
		}
	}
	return min_height+border*2;
}

void aContainer::move() {
	aWidget::move();
	if(child)
		for(aWidget *w=child; w; w=w->next)
			w->move();
}
#endif


