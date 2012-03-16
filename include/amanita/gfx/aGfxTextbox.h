#ifndef _AMANITA_GFX_TEXTBOX_H
#define _AMANITA_GFX_TEXTBOX_H

/**
 * @file amanita/gfx/aGfxTextbox.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxFont.h>
#include <amanita/gfx/aGfxWindow.h>


enum {
	TEXTBOX_YES				= 0x00008061,
	TEXTBOX_YES_NO			= 0x00008062,
	TEXTBOX_OK				= 0x00008063,
	TEXTBOX_OK_CANCEL		= 0x00008064,
};


/** The aGfxTextbox is a modal dialog that shows a message.
 * @ingroup sdl
 */
class aGfxTextbox : public aGfxWindow {
aObject_Instance(aGfxTextbox)

private:
	unsigned int style;

	aGfxTextbox(int w,int h,const char *msg,unsigned int st,aGfxFont *font);

	static void hideMessage(aGfxTextbox *m,int id);

public:
	static char yes[16],no[16],ok[16],cancel[16];

	static void showMessage(const char *msg,unsigned int st,aGfxActionListener *al,aGfxFont *font=0);

	bool keyDown(aGfxKeyEvent &ke);
	bool actionPerformed(aGfxActionEvent &ae);
};


#endif /* _AMANITA_GFX_TEXTBOX_H */
