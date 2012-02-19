#ifndef _AMANITA_GUI_MENU_H
#define _AMANITA_GUI_MENU_H

/**
 * @file amanita/gui/aMenu.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <stdio.h>
#include <amanita/gui/aWidget.h>


enum {
	aMENU_EVENT_ACTION,
	aMENU_EVENT_ENTER,
	aMENU_EVENT_LEAVE,
};


class aMenu;

struct aMenuItem {
	int pid;
	int id;
	const char *name;
	bool sensitive;
	int acc;
	int acc_mod;
	int index;
	int lvl;
	aMenuItem *parent;
	aMenuItem *child;
	aMenu *menu;
	aComponent item;
	aComponent submenu;
};

class aMenu : public aWidget {
friend class aApplication;
friend class aWindow;

/** @cond */
aObject_Instance(aMenu)
/** @endcond */

private:
	aMenuItem *items;
	aMenuItem **items_index;
	int nitems;

public:
	aMenu(widget_event_handler weh,const aMenuItem *mi);
	virtual ~aMenu();

	aMenuItem *getItem(int n) { return n>=0 && n<nitems? items_index[n] : 0; }

	virtual void create(aWindow *wnd,uint32_t st);
};

/*
class aMenu {
private:
	HMENU menu;
	unsigned int id,flags,sz;
	char *cont,*tit;
	aMenu **items;

	void create(HMENU m);

public:
	aMenu(unsigned int f,unsigned int id,char *c=0,char *t=0,aMenu **m=0);
	~aMenu();
	HMENU createMenu();
	static aMenu *read(FILE *fp);

	char *getTitle(unsigned int n);
};
*/


#endif /* _AMANITA_GUI_MENU_H */

