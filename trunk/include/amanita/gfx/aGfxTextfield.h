#ifndef _AMANITA_GFX_TEXTFIELD_H
#define _AMANITA_GFX_TEXTFIELD_H

/**
 * @file amanita/gfx/aGfxTextfield.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxText.h>
#include <amanita/gfx/aGfxComponent.h>

class aGfxFont;

/** The aGfxTextfield class inherits the aGfxText class to handle multiline text, and show within a frame.
 * @ingroup sdl
 */
class aGfxTextfield : public aGfxComponent,public aGfxText,public aGfxKeyListener {
Object_Instance(aGfxTextfield)

private:
	struct _settings {
		aGfxFont *font;
	};
	static _settings _s;

	char align,showCaret,caretTimer;

public:
	static void setDefaultSettings(aGfxFont *f);

	aGfxTextfield(int x=0,int y=0,int w=0,int h=0,const char *str=0);

	void setTextAlign(int a) { align = a; }
	bool keyDown(aGfxKeyEvent &ke);
	void paint(time_t time);
	void setShowCaret(bool c) { showCaret = c; }
};



#endif /* _AMANITA_GFX_TEXTFIELD_H */
