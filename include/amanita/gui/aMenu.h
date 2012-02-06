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
	MENU_EVENT_ACTION,
	MENU_EVENT_ENTER,
	MENU_EVENT_LEAVE,
};


class aMenu;

struct aMenuItem {
	int pid;
	int id;
	const char *name;
	const char *status;
	int action;
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
friend class aWindow;
private:
	aMenuItem *items;
	aMenuItem **items_index;
	int nitems;

public:
	aMenu(widget_event_handler weh,const aMenuItem *mi);
	virtual ~aMenu();

	virtual aComponent create();
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

