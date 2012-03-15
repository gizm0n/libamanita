#ifndef _AMANITA_GUI_WINDOW_H
#define _AMANITA_GUI_WINDOW_H

/**
 * @file amanita/gui/aWindow.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <amanita/gui/aContainer.h>

enum {
	aWINDOW_DIALOG				= 0x00000001,
	aWINDOW_RESIZABLE			= 0x00000002,
	aWINDOW_MODAL				= 0x00000004,
	aWINDOW_FULLSCREEN		= 0x00000008,
	aWINDOW_CENTER				= 0x00000010,
};

enum {
	aWINDOW_APP_ICON			= 101,
};


class aApplication;
class aMenu;
class aStatus;

class aWindow : public aContainer {
friend class aMenu;
friend class aWidget;
/** @cond */
#ifdef USE_WIN32
friend LRESULT CALLBACK AmanitaMainWndProc(HWND,UINT,WPARAM,LPARAM);
#endif

aObject_Instance(aWindow)
/** @endcond */

private:
	aApplication *app;
	aMenu *menu;
	aStatus *status;

#ifdef USE_GTK
	GtkWidget *wnd;
#endif

	void openWindow(aWindow *wnd);
	void closeWindow(aWindow *wnd);

#ifdef USE_WIN32
	static bool handleEvent(aWindow *wnd,UINT msg,WPARAM wparam,LPARAM lparam);
	bool command(WPARAM wparam,LPARAM lparam);
	bool notify(LPNMHDR nmhdr);
	bool drawItem(LPDRAWITEMSTRUCT dis);

	virtual void makeLayout(int x,int y,int w,int h);
#endif

public:
	aWindow(aApplication *a,widget_event_handler weh);
	virtual ~aWindow();

	void open(aWindow *p);
	bool close();

	aApplication *getApplication() { return app; }
	void setMenu(aMenu *m);
	aMenu *getMenu() { return menu; }
	void setStatus(aStatus *sb);
	void updateStatus(int n,const char *format, ...);
	aStatus *getStatus() { return status; }

	virtual void create(aWindow *wnd,uint32_t st);
	virtual void createAll(aComponent p,bool n);
};


#endif /* _AMANITA_GUI_WINDOW_H */

