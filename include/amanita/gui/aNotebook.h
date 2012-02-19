#ifndef _AMANITA_GUI_NOTEBOOK_H
#define _AMANITA_GUI_NOTEBOOK_H

/**
 * @file amanita/gui/aNotebook.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-07
 * @date Created: 2012-02-07
 */ 

#include <amanita/aVector.h>
#include <amanita/gui/aWidget.h>


enum {
	aNOTEBOOK_CLOSE_BUTTON	= 0x00000001,
};

enum {
	aNOTEBOOK_PAGE_SHOW		= 0x00000001,
	aNOTEBOOK_PAGE_HIDE		= 0x00000002,
};


class aNotebook : public aWidget {
friend class aWindow;

/** @cond */
aObject_Instance(aNotebook)
/** @endcond */

private:
	aVector pages;
	int selected;

#ifdef USE_WIN32
/** @cond */
	RECT client;

	void tabEvent();
/** @endcond */
	virtual void makeLayout(int x,int y,int w,int h);
	virtual void move();
#endif

public:
	aNotebook(widget_event_handler weh);
	virtual ~aNotebook();

	virtual void create(aWindow *wnd,uint32_t st);

	int openPage(const char *tab,aWidget *page);
	void selectPage(int n);
	const char *getTab(int n);
	aWidget *getPage(int n);
	void closePage(int n);
};


#endif /* _AMANITA_GUI_NOTEBOOK_H */

