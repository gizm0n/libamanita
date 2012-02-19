#ifndef _AMANITA_GUI_STATUSBAR_H
#define _AMANITA_GUI_STATUSBAR_H

/**
 * @file amanita/gui/aStatusbar.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-06
 * @date Created: 2012-02-06
 */ 

#include <stdio.h>
#include <amanita/gui/aWidget.h>


class aStatusbar : public aWidget {
friend class aWindow;

/** @cond */
aObject_Instance(aStatusbar)
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
	aStatusbar(const int c[]);
	virtual ~aStatusbar();

	virtual void create(aWindow *wnd,uint32_t st);

	void setStatus(int n,const char *format, ...);
};

#endif /* _AMANITA_GUI_STATUSBAR_H */

