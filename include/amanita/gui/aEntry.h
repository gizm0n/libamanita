#ifndef _AMANITA_GUI_ENTRY_H
#define _AMANITA_GUI_ENTRY_H

/**
 * @file amanita/gui/aEntry.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-14
 * @date Created: 2012-02-14
 */ 


#include <amanita/gui/aWidget.h>


class aEntry : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
aObject_Instance(aEntry)
/** @endcond */

public:	
	aEntry(widget_event_handler weh);
	virtual ~aEntry();

	virtual void create(aWindow *wnd,uint32_t st);

	void setText(const char *str);
};


#endif /* _AMANITA_GUI_ENTRY_H */

