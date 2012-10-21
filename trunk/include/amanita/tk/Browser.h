#ifndef _AMANITA_TK_BROWSER_H
#define _AMANITA_TK_BROWSER_H

/**
 * @file amanita/tk/Browser.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-01-28
 */ 

#include <stdint.h>
#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	BROWSER_ENABLE_CONTEXT_MENU	= 0x00000001,
};

enum {
	BROWSER_CREATE						= 0x00000001,
	BROWSER_CLICK						= 0x00000011,
	BROWSER_REDIRECT					= 0x00000012,
	BROWSER_LOADED						= 0x00000013,
	BROWSER_CONTEXT_MENU				= 0x00000014,
	BROWSER_UPDATE_STATUS			= 0x00000015,
};


/** @cond */
#ifdef USE_WIN32
class _EventSink;
class _Container;
#endif
/** @endcond */


/** This widget is a complete web browser.
 * 
 * @ingroup tk */
class Browser : public Widget {
/** @cond */
#ifdef USE_WIN32
friend LRESULT CALLBACK AmanitaBrowserProc(HWND,UINT,WPARAM,LPARAM);
friend class _EventSink;
friend class _Container;
#endif
Object_Instance(Browser)
/** @endcond */

private:
/** @cond */
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
	Browser(widget_event_handler weh);
	virtual ~Browser();

	virtual void create(Window *wnd,uint32_t st);

	bool handleEvent(int msg,const char *url);

	void setUrl(const char *url);
	void setHtmlContent(const char *html);
#ifdef USE_WIN32
	void setUrl(const wchar_t *url);
	void setHtmlContent(const wchar_t *html);
#endif
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_BROWSER_H */

