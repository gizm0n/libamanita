#ifndef _AMANITA_GUI_CONTAINER_H
#define _AMANITA_GUI_CONTAINER_H

/**
 * @file amanita/gui/aContainer.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-01
 * @date Created: 2012-02-10
 */ 

#include <amanita/gui/aWidget.h>


enum {
	/** Win32 specific, if set will create a Static on which children are placed, otherwise
	 * this widget is purely wirtual and only used for layout management, and children are
	 * placed on the first solid parent componenet in the hierarchy. */
	aCONTAINER_HARD				= 0x00000001,
};


class aContainer : public aWidget {
friend class aWindow;

/** @cond */
aObject_Instance(aContainer)
/** @endcond */

protected:
	aWidget *child;
	uint8_t children;

	aContainer(widget_event_handler weh,widget_type t);

#ifdef USE_WIN32
	virtual void makeLayout(int x,int y,int w,int h);
	virtual int getMinimumWidth();
	virtual int getMinimumHeight();
	virtual void move();
#endif

public:
	aContainer();
	virtual ~aContainer();

	void add(aWidget *w);
	aWidget *remove(aWidget *w);
	aWidget *remove(uint32_t id);

	virtual void create(aWindow *wnd,uint32_t st);
	virtual void createAll(aComponent p,bool n);
};


#endif /* _AMANITA_GUI_CONTAINER_H */

