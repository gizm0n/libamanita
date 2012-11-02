#ifndef _AMANITA_TK_MENU_H
#define _AMANITA_TK_MENU_H

/**
 * @file amanita/tk/Menu.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-01-28
 */ 

#include <stdio.h>
#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


/** @cond */
class Application;
/** @endcond */


enum {
	MENU_EVENT_ACTION,
	MENU_EVENT_ENTER,
	MENU_EVENT_LEAVE,
};


class Menu;


/** Menu item.
 * @ingroup tk */
struct MenuItem {
	int pid;
	int id;
	const char *name;
	bool sensitive;
	int acc;
	int acc_mod;
	const char *icon;
	void *data;
	int index;
	int lvl;
	MenuItem *parent;
	MenuItem *child;
	Menu *menu;
	Component item;
	Component submenu;
};


/** Menu widget.
 * 
 * @ingroup tk */
class Menu : public Widget {
friend class Application;
friend class Window;
/** @cond */
Object_Instance(Menu)
/** @endcond */

private:
	MenuItem *items;
	MenuItem **items_index;
	int nitems;

public:
	Menu(widget_event_handler weh,const MenuItem *mi);
	virtual ~Menu();

	MenuItem *getItem(int n) { return n>=0 && n<nitems? items_index[n] : 0; }

	virtual void create(Window *wnd,uint32_t st);
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_MENU_H */

