#ifndef _AMANITA_GUI_STATUSBAR_H
#define _AMANITA_GUI_STATUSBAR_H

/**
 * @file amanita/gui/aStatus.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-06
 * @date Created: 2012-02-06
 */ 

#include <stdio.h>
#include <amanita/gui/aWidget.h>


class aStatus : public aWidget {
friend class aWindow;

/** @cond */
Object_Instance(aStatus)
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
	aStatus(const int c[]);
	virtual ~aStatus();

	virtual void create(aWindow *wnd,uint32_t st);

	void update(int n,const char *str);
};

#endif /* _AMANITA_GUI_STATUSBAR_H */

