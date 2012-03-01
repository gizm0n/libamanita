#ifndef _AMANITA_GUI_ENTRY_H
#define _AMANITA_GUI_ENTRY_H

/**
 * @file amanita/gui/aText.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-14
 * @date Created: 2012-02-14
 */ 


#include <amanita/gui/aWidget.h>


enum {
	aENTRY_MULTILINE			= 0x00000001,
};


class aText : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
aObject_Instance(aText)
/** @endcond */

public:	
	aText(widget_event_handler weh);
	virtual ~aText();

	virtual void create(aWindow *wnd,uint32_t st);

	void setText(const char *str);
};


#endif /* _AMANITA_GUI_ENTRY_H */

