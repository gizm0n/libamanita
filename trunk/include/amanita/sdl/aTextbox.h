#ifndef _AMANITA_SDL_TEXTBOX_H
#define _AMANITA_SDL_TEXTBOX_H

/**
 * @file amanita/sdl/aTextbox.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aFont.h>
#include <amanita/sdl/aWindow.h>


enum {
	TEXTBOX_YES				= 0x00008061,
	TEXTBOX_YES_NO			= 0x00008062,
	TEXTBOX_OK				= 0x00008063,
	TEXTBOX_OK_CANCEL		= 0x00008064,
};


/** The aTextbox is a modal dialog that shows a message.
 * @ingroup sdl
 */
class aTextbox : public aWindow {
aObject_Instance(aTextbox)

private:
	unsigned int style;

	aTextbox(int w,int h,const char *msg,unsigned int st,aFont *font);

	static void hideMessage(aTextbox *m,int id);

public:
	static char yes[16],no[16],ok[16],cancel[16];

	static void showMessage(const char *msg,unsigned int st,aActionListener *al,aFont *font=0);

	bool keyDown(aKeyEvent &ke);
	bool actionPerformed(aActionEvent &ae);
};


#endif /* _AMANITA_SDL_TEXTBOX_H */
