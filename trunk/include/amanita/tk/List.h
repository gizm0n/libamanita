#ifndef _AMANITA_TK_LIST_H
#define _AMANITA_TK_LIST_H

/**
 * @file amanita/tk/List.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-13
 */ 


#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	LIST_CHANGED			= 0x00000001,
	LIST_ACTIVATE			= 0x00000002,
};


/** List widget.
 * 
 * @ingroup tk */
class List : public Widget {
friend class Container;
friend class Window;
/** @cond */
Object_Instance(List)
/** @endcond */

private:
/** @cond */
#ifdef USE_GTK
	GtkWidget *list;
	GtkListStore *store;
	GtkTreeIter iter;
#endif
/** @endcond */

	char **columns;
	int *column_widths;
	int ncolumns;
	char *item;
	int selected;

public:	
	List(widget_event_handler weh);
	virtual ~List();

	virtual void create(Window *wnd,uint32_t st);

	void setColumns(const char *cols,const int widths[],char sep='|');
	void addRow(const char *cols,char sep='|');
	const char *getColumn(int n);
};


}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_LIST_H */

