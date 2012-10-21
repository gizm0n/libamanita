#ifndef _AMANITA_TK_CONTAINER_H
#define _AMANITA_TK_CONTAINER_H

/**
 * @file amanita/tk/Container.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-10
 */ 

#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


/** Container base class.
 * 
 * All container classes inherit this class.
 * 
 * @ingroup tk */
class Container : public Widget {
friend class Window;
/** @cond */
Object_Instance(Container)
/** @endcond */

protected:
	Widget *child;
	uint8_t children;

	Container(widget_event_handler weh,widget_type t);

#ifdef USE_WIN32
	virtual void makeLayout(int x,int y,int w,int h);
	virtual int getMinimumWidth();
	virtual int getMinimumHeight();
	virtual void move();
#endif

public:
	Container();
	virtual ~Container();

	void add(Widget *w);
	Widget *remove(Widget *w);
	Widget *remove(uint32_t id);

	virtual void create(Window *wnd,uint32_t st);
	virtual void createAll(Component p,bool n);
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_CONTAINER_H */

