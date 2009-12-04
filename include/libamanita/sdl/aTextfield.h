#ifndef _LIBAMANITA_SDL_TEXTFIELD_H
#define _LIBAMANITA_SDL_TEXTFIELD_H

/**
 * @file libamanita/sdl/aTextfield.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/sdl/aText.h>
#include <libamanita/sdl/aComponent.h>

class aFont;

class aTextfield : public aComponent,public aText,public aKeyListener {
RttiObjectInstance(aTextfield)

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



#endif /* _LIBAMANITA_SDL_TEXTFIELD_H */
