
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/tk/Container.h>
#include <amanita/tk/Window.h>



namespace a {
namespace tk {


Object_Inheritance(Container,Widget)

Container::Container() : Widget(0,WIDGET_CONTAINER) {
	child = 0;
	children = 0;
}

Container::Container(widget_event_handler weh,widget_type t) : Widget(weh,t) {
	child = 0;
	children = 0;
}

Container::~Container() {
	if(child) delete child;
	child = 0;
	children = 0;
}

void Container::add(Widget *w) {
	if(children==0xff) return; // Will only accept up to 255 children.
debug_output("Container::add(w=%p)\n",w);
	w->parent = this;
	if(!child) child = w;
	else {
		Widget *c;
		for(c=child; c->next; c=c->next);
		c->next = w;
	}
	++children;
debug_output("Container::add(child=%p)\n",child);
}

Widget *Container::remove(Widget *w) {
	return 0;
}

Widget *Container::remove(uint32_t id) {
	return 0;
}


void Container::create(Window *wnd,uint32_t st) {
	if(text) type = WIDGET_FRAME;
	else if(parent && *parent==Container::getClass()) type = WIDGET_VOID;
debug_output("Container::create(type: %d)\n",type);

#ifdef USE_GTK
	if((style&HORIZONTAL)) {
		component = (Component)gtk_hbox_new(
			(style&HOMOGENOUS)? TRUE : FALSE,spacing);
	} else if((style&VERTICAL)) {
		component = (Component)gtk_vbox_new(
			(style&HOMOGENOUS)? TRUE : FALSE,spacing);
	} else if((style&TABLE)) {
	}
	if(border) gtk_container_set_border_width(GTK_CONTAINER(component),border);
#endif

	Widget::create(wnd,0);

#ifdef USE_WIN32
	captureEvents();
#endif
}

void Container::createAll(Component p,bool n) {
	Widget *w;
debug_output("Container::createAll(p: %p)\n",p);
#ifdef USE_GTK
	if(child) {
		for(w=child; w; w=w->next) {
			w->create(window,0);
			w->createAll(0,false);
			if((style&HORIZONTAL) || (style&VERTICAL)) {
				gtk_box_pack_start(GTK_BOX(component),(GtkWidget *)w->component,
					(w->style&EXPAND)? TRUE : FALSE,
					(w->style&FILL)? TRUE : FALSE,
					0);
			} else if((style&TABLE)) {
			}
		}
		if(type==WIDGET_FRAME) {
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
debug_output("Container::createAll()\n");
	if(n && next) {
		for(w=next; w; w=w->next) {
			w->create(window,0);
			w->createAll(p,false);
		}
	}
	if(child) {
		if(type==WIDGET_VOID) {
			Widget *wp = parent;
			while(wp && wp->type==WIDGET_VOID) wp = wp->parent;
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
void Container::makeLayout(int x,int y,int w,int h) {
debug_output("Container::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",x,y,w,h);
	Widget::makeLayout(x,y,w,h);
//	if(border) this->x += border,this->y += border,width -= border*2,height -= border*2;
	if(child) {
		Widget *c = child;
		int i,n = 0,sz[children],min,max,sum,exp,nexp,sp = (children-1)*spacing;
		int x1 = 0,y1 = 0,w1 = 0,h1 = 0,w2 = width,h2 = height;
		if(type==WIDGET_VOID) x1 += this->x,y1 += this->y;
		else if(type==WIDGET_FRAME)
			x1 += MulDiv(dbu_x,7,4),y1 += MulDiv(dbu_y,11,8),w2 -= MulDiv(dbu_x,7+7,4),h1 -= MulDiv(dbu_y,11+7,8);
		if((style&HORIZONTAL)) {
			for(i=0,c=child,min=0,max=0,sum=0,exp=0,nexp=0; c; ++i,c=c->next) {
				sum += sz[i] = n = c->getMinimumWidth();
				if(!min || min>n) min = n;
				if(max<n) max = n;
				if((c->style&EXPAND)) ++exp;
				else nexp += n;
			}
debug_output("Container::makeLayout(min: %d, max: %d, exp: %d, nexp: %d)\n",min,max,exp,nexp);
			if((style&HOMOGENOUS)) {
				for(w1=(w2-sp)/children,c=child; c; x1+=w1+spacing,c=c->next) {
					if(!c->next) w1 = w2-x1;
debug_output("Container::makeLayout(HOMOGENOUS x1: %d, w1: %d  %s)\n",x1,w1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w1,h2);
				}
			} else {
				for(i=0,c=child; c; ++i,x1+=w1+spacing,c=c->next) {
					if((c->style&EXPAND)) {
						if(!c->next) w1 = w2-x1;
						else w1 = (w2-sp-nexp)/exp;
					} else w1 = sz[i];
debug_output("Container::makeLayout(x1: %d, w1: %d  %s)\n",x1,w1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w1,h2);
				}
			}

		} else if((style&VERTICAL)) {
			for(i=0,c=child,min=0,max=0,sum=0,exp=0,nexp=0; c; ++i,c=c->next) {
				sum += sz[i] = n = c->getMinimumHeight();
				if(!min || min>n) min = n;
				if(max<n) max = n;
				if((c->style&EXPAND)) ++exp;
				else nexp += n;
			}
debug_output("Container::makeLayout(min: %d, max: %d, exp: %d, nexp: %d)\n",min,max,exp,nexp);
			if((style&HOMOGENOUS)) {
				for(h1=(h2-sp)/children,c=child; c; y1+=n+spacing,c=c->next) {
					if(!c->next) h1 = h2-y1;
debug_output("Container::makeLayout(HOMOGENOUS y1: %d, h1: %d  %s)\n",y1,h1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w2,h1);
				}
			} else {
				for(i=0,c=child; c; ++i,y1+=h1+spacing,c=c->next) {
					if((c->style&EXPAND)) {
						if(!c->next) h1 = h2-y1;
						else h1 = (h2-sp-nexp)/exp;
					} else h1 = sz[i];
debug_output("Container::makeLayout(y1: %d, h1: %d  %s)\n",y1,h1,c->text? c->text : "-");
					c->makeLayout(x1,y1,w2,h1);
				}
			}
		} else if((style&TABLE)) {
			
		} else {
			for(; c; c=c->next)
				c->makeLayout(x,y,w,h);
		}
	}
}

int Container::getMinimumWidth() {
	if(!(style&FIXED)) {
		Widget *w;
		int box = 0;
		if(type==WIDGET_FRAME) box = MulDiv(dbu_x,7+7,4);
		if((style&HORIZONTAL)) {
			int n = border*2+(children-1)*spacing;
			for(w=child; w; w=w->next) n += w->getMinimumWidth();
			return box+n;
		} else if((style&VERTICAL)) {
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

int Container::getMinimumHeight() {
	if(!(style&FIXED)) {
		Widget *w;
		int box = 0;
		if(type==WIDGET_FRAME) box = MulDiv(dbu_y,11+7,8);
		if((style&VERTICAL)) {
			int n = border*2+(children-1)*spacing;
			for(w=child; w; w=w->next) n += w->getMinimumHeight();
			return box+n;
		} else if((style&HORIZONTAL)) {
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

void Container::move() {
	Widget::move();
	if(child)
		for(Widget *w=child; w; w=w->next)
			w->move();
}
#endif


}; /* namespace tk */
}; /* namespace a */


