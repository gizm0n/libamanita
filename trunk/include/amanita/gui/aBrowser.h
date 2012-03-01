#ifndef _AMANITA_GUI_BROWSER_H
#define _AMANITA_GUI_BROWSER_H

/**
 * @file amanita/gui/aBrowser.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <stdint.h>
#include <amanita/gui/aWidget.h>

enum {
	aBROWSER_ENABLE_CONTEXT_MENU	= 0x00000001,
};

enum {
	aBROWSER_CREATE					= 0x00000001,
	aBROWSER_CLICK						= 0x00000011,
	aBROWSER_REDIRECT					= 0x00000012,
	aBROWSER_LOADED					= 0x00000013,
	aBROWSER_CONTEXT_MENU			= 0x00000014,
	aBROWSER_UPDATE_STATUS			= 0x00000015,
};

#ifdef USE_WIN32
class _EventSink;
class _Container;
#endif


class aBrowser : public aWidget {
/** @cond */
#ifdef USE_WIN32
friend LRESULT CALLBACK AmanitaBrowserProc(HWND,UINT,WPARAM,LPARAM);
friend class _EventSink;
friend class _Container;
#endif

aObject_Instance(aBrowser)

private:
#ifdef USE_GTK
	GtkWidget *webkit;
#endif
#ifdef USE_WIN32
	_Container *_container;
	intptr_t stamp;

	bool pumpMessages(intptr_t st);
#endif
/** @endcond */

public:
	aBrowser(widget_event_handler weh);
	virtual ~aBrowser();

	virtual void create(aWindow *wnd,uint32_t st);

	bool handleEvent(int msg,const char *url);

	void setUrl(const char *url);
	void setHtmlContent(const char *html);
#ifdef USE_WIN32
	void setUrl(const wchar_t *url);
	void setHtmlContent(const wchar_t *html);
#endif
};


#endif /* _AMANITA_GUI_BROWSER_H */

