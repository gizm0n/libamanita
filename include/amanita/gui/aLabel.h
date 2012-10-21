#ifndef _AMANITA_GUI_LABEL_H
#define _AMANITA_GUI_LABEL_H

/**
 * @file amanita/gui/aLabel.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-13
 * @date Created: 2012-02-13
 */ 


#include <amanita/gui/aWidget.h>


enum {
	aLABEL_LEFT					= 0x00000001,
	aLABEL_RIGHT				= 0x00000002,
	aLABEL_CENTER				= 0x00000003,
	aLABEL_TOP					= aTOP,
	aLABEL_BOTTOM				= aBOTTOM,
	aLABEL_MIDDLE				= aMIDDLE,
	aLABEL_ALIGNED				= 0x0000000f,
};


class aLabel : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
Object_Instance(aLabel)
/** @endcond */

public:	
	aLabel(widget_event_handler weh);
	virtual ~aLabel();

	virtual void create(aWindow *wnd,uint32_t st);

	void setText(const char *str);
};


#endif /* _AMANITA_GUI_LABEL_H */

