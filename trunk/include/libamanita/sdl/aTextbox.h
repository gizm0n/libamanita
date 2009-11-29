#ifndef _LIBAMANITA_SDL_ATEXTBOX_H
#define _LIBAMANITA_SDL_ATEXTBOX_H

#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aWindow.h>


enum {
	TEXTBOX_YES				= 0x00008061,
	TEXTBOX_YES_NO			= 0x00008062,
	TEXTBOX_OK				= 0x00008063,
	TEXTBOX_OK_CANCEL		= 0x00008064,
};


class aTextbox : public aWindow {
RttiObjectInstance(aTextbox)

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


#endif /* _LIBAMANITA_SDL_ATEXTBOX_H */
