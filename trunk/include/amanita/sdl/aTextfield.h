#ifndef _AMANITA_SDL_TEXTFIELD_H
#define _AMANITA_SDL_TEXTFIELD_H

/**
 * @file amanita/sdl/aTextfield.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aText.h>
#include <amanita/sdl/aComponent.h>

class aFont;

/** The aTextfield class inherits the aText class to handle multiline text, and show within a frame.
 * @ingroup sdl
 */
class aTextfield : public aComponent,public aText,public aKeyListener {
aObject_Instance(aTextfield)

private:
	struct _settings {
		aFont *font;
	};
	static _settings _s;

	char align,showCaret,caretTimer;

public:
	static void setDefaultSettings(aFont *f);

	aTextfield(int x=0,int y=0,int w=0,int h=0,const char *str=0);

	void setTextAlign(int a) { align = a; }
	bool keyDown(aKeyEvent &ke);
	void paint(time_t time);
	void setShowCaret(bool c) { showCaret = c; }
};



#endif /* _AMANITA_SDL_TEXTFIELD_H */
