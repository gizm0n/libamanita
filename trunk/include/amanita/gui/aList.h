#ifndef _AMANITA_GUI_LIST_H
#define _AMANITA_GUI_LIST_H

/**
 * @file amanita/gui/aList.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-13
 * @date Created: 2012-02-13
 */ 


#include <amanita/gui/aWidget.h>


enum {
	aLIST_CHANGED			= 0x00000001,
	aLIST_ACTIVATE			= 0x00000002,
};


class aList : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
Object_Instance(aList)
/** @endcond */

private:
	char **columns;
	int *column_widths;
	int ncolumns;
	char *item;
	int selected;

#ifdef USE_GTK
	GtkWidget *list;
	GtkListStore *store;
	GtkTreeIter iter;
#endif

public:	
	aList(widget_event_handler weh);
	virtual ~aList();

	virtual void create(aWindow *wnd,uint32_t st);

	void setColumns(const char *cols,const int widths[],char sep='|');
	void addRow(const char *cols,char sep='|');
	const char *getColumn(int n);
};


#endif /* _AMANITA_GUI_LIST_H */

