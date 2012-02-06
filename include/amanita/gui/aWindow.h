#ifndef _AMANITA_GUI_WINDOW_H
#define _AMANITA_GUI_WINDOW_H

/**
 * @file amanita/gui/aWindow.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <amanita/gui/aWidget.h>


class aApplication;
class aMenu;
class aStatusbar;

#ifdef WIN32
#define AMANITA_MAIN_WINDOW_CLASS "AmanitaMainWndClass"
#endif

class aWindow : public aWidget {
#ifdef WIN32
friend LRESULT CALLBACK AmanitaMainWndProc(HWND,UINT,WPARAM,LPARAM);
#endif

private:
	aApplication *app;
	aWindow *window;
	aMenu *menu;
	aStatusbar *statusbar;
#ifdef WIN32
//	INT_PTR dlg_result;
#endif

public:
	aWindow(aApplication *a,widget_event_handler weh);
	~aWindow();

	void open();
	bool close();

	void setMenu(aMenu *m);
	aMenu *getMenu() { return menu; }
	void setStatusbar(aStatusbar *sb);
	aStatusbar *getStatusbar() { return statusbar; }
	void openWindow(aWindow *w) {}
	void closeWindow(aWindow *w) {}

	virtual aComponent create();

#ifdef WIN32
	virtual void makeLayout(int w,int h);
#endif
};


#endif /* _AMANITA_GUI_WINDOW_H */

