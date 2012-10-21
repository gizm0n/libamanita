#ifndef _AMANITA_TK_WINDOW_H
#define _AMANITA_TK_WINDOW_H

/**
 * @file amanita/tk/Window.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-01-28
 */ 

#include <amanita/Application.h>
#include <amanita/Resource.h>
#include <amanita/tk/Container.h>

/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


/** @cond */
class Menu;
class Status;
/** @endcond */


enum {
	WINDOW_DIALOG				= 0x00000001,
	WINDOW_RESIZABLE			= 0x00000002,
	WINDOW_MODAL				= 0x00000004,
	WINDOW_FULLSCREEN			= 0x00000008,
	WINDOW_CENTER				= 0x00000010,
};


/** Window icon.
 * @ingroup tk */
struct WindowIcon {
	int size;
	int id;
	const char **xpm;
	const char *png;
};


/** Window widget.
 * 
 * @ingroup tk */
class Window : public Container {
friend class Menu;
friend class Widget;
/** @cond */
#ifdef USE_WIN32
friend LRESULT CALLBACK AmanitaMainWndProc(HWND,UINT,WPARAM,LPARAM);
#endif
Object_Instance(Window)
/** @endcond */

private:
	Application *app;
	Menu *menu;
	Status *status;

	WindowIcon *icons;

/** @cond */
#ifdef USE_GTK
	GtkWidget *wnd;
#endif
/** @endcond */

	void openWindow(Window *wnd);
	void closeWindow(Window *wnd);

#ifdef USE_WIN32
	static bool handleEvent(Window *wnd,UINT msg,WPARAM wparam,LPARAM lparam);
	bool command(WPARAM wparam,LPARAM lparam);
	bool notify(LPNMHDR nmhdr);
	bool drawItem(LPDRAWITEMSTRUCT dis);

	virtual void makeLayout(int x,int y,int w,int h);
#endif

	void clearIcons();

public:
	Window(Application *a,widget_event_handler weh);
	virtual ~Window();

	void open(Window *p);
	bool close();

	Application *getApplication() { return app; }
	void setMenu(Menu *m);
	Menu *getMenu() { return menu; }
	void setStatus(Status *sb);
	void updateStatus(int n,const char *format, ...);
	Status *getStatus() { return status; }

	void setIcons(const WindowIcon icons[]);

	virtual void create(Window *wnd,uint32_t st);
	virtual void createAll(Component p,bool n);
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_WINDOW_H */

