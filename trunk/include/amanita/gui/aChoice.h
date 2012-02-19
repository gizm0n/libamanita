#ifndef _AMANITA_GUI_CHOICE_H
#define _AMANITA_GUI_CHOICE_H

/**
 * @file amanita/gui/aChoice.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-13
 * @date Created: 2012-02-13
 */ 


#include <amanita/aVector.h>
#include <amanita/gui/aWidget.h>


enum {
	aCHOICE_COMBOBOX		= 0x00000001,
	aCHOICE_ENTRY			= 0x00000002,
};

enum {
	aCHOICE_CHANGED		= 0x00000001,
};


class aChoice : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
aObject_Instance(aChoice)
/** @endcond */

private:
	aVector *items;
	char *item;
	int selected;

public:	
	aChoice(widget_event_handler weh);
	virtual ~aChoice();

	virtual void create(aWindow *wnd,uint32_t st);

	void setItems(const char *arr[]);
	void addItem(const char *str);
	void select(int n);
	int getSelected();
	const char *getItem();
};


#endif /* _AMANITA_GUI_CHOICE_H */

