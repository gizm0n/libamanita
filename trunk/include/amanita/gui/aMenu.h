#ifndef _AMANITA_GUI_MENU_H
#define _AMANITA_GUI_MENU_H

/**
 * @file amanita/gui/aMenu.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <amanita/gui/aWidget.h>

class aMenu;

struct menu_item {
	int pid;
	int id;
	int action;
	bool sensitive;
	int acc;
	int acc_mod;
	int index;
	int lvl;
	const char *name;
	const char *status;
	menu_item *parent;
	menu_item *child;
	aMenu *menu;
	aHandle item;
	aHandle submenu;
};

class aMenu : public aWidget {
private:
	menu_item *items;
	menu_item **items_index;
	int nitems;
//	aHashtable menu;

	static gboolean menuitem_callback(GtkWidget *widget,GdkEventCrossing *ev,gpointer data);

public:
	aMenu(widget_event_handler weh,const menu_item *mi);
	virtual ~aMenu();

	virtual aHandle create(aHandle p,int s); // <-- Move to private...
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

