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


#ifdef WIN32
#define BROWSER_CLASS "WebControl32"

enum {
	BROWSER_CREATE				= 0x01,
	BROWSER_ONCLICK			= 0x11,
	BROWSER_ONREDIRECT		= 0x12,
	BROWSER_ONLOADED			= 0x13,
};

class _EventSink;
class _Container;
#endif


class aBrowser : public aWidget {
#ifdef WIN32
friend LRESULT CALLBACK AmanitaBrowserProc(HWND,UINT,WPARAM,LPARAM);
friend class _EventSink;
friend class _Container;
private:
	_Container *container;
	intptr_t stamp;

	bool pumpMessages(intptr_t st);
#endif

public:
	aBrowser(widget_event_handler weh);
	virtual ~aBrowser();

	virtual aComponent create();

	void setUrl(const char *url);
	void setHtmlContent(const char *html);
};


#endif /* _AMANITA_GUI_BROWSER_H */

