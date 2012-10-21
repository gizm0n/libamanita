#ifndef _AMANITA_TK_STATUSBAR_H
#define _AMANITA_TK_STATUSBAR_H

/**
 * @file amanita/tk/Status.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-06
 */ 

#include <stdio.h>
#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


/** Status widget.
 * 
 * @ingroup tk */
class Status : public Widget {
friend class Window;
/** @cond */
Object_Instance(Status)
/** @endcond */

private:
	int *cells;
	int ncells;
/** @cond */
#ifdef USE_GTK
	GtkWidget **_cells;
#endif
/** @endcond */

public:
	Status(const int c[]);
	virtual ~Status();

	virtual void create(Window *wnd,uint32_t st);

	void update(int n,const char *str);
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_STATUSBAR_H */

