#ifndef _AMANITA_GUI_SELECT_H
#define _AMANITA_GUI_SELECT_H

/**
 * @file amanita/gui/aSelect.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-13
 * @date Created: 2012-02-13
 */ 


#include <amanita/Vector.h>
#include <amanita/gui/aWidget.h>


enum {
	aSELECT_COMBOBOX		= 0x00000001,
	aSELECT_ENTRY			= 0x00000002,
};

enum {
	aSELECT_CHANGED		= 0x00000001,
};


class aSelect : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
Object_Instance(aSelect)
/** @endcond */

private:
	Vector *items;
	char *item;
	int selected;

public:	
	aSelect(widget_event_handler weh);
	virtual ~aSelect();

	virtual void create(aWindow *wnd,uint32_t st);

	void setItems(const char *arr[]);
	void addItem(const char *str);
	void select(int n);
	int getSelected();
	const char *getItem();
};


#endif /* _AMANITA_GUI_SELECT_H */

