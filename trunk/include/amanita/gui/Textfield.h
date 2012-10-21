#ifndef _AMANITA_GUI_TEXTFIELD_H
#define _AMANITA_GUI_TEXTFIELD_H

/**
 * @file amanita/gui/Textfield.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/gui/Text.h>
#include <amanita/gui/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Font;
/** @endcond */


/** The Textfield class inherits the Text class to handle multiline text, and show within a frame.
 * 
 * @ingroup gui */
class Textfield : public Widget,public Text,public KeyListener {
/** @cond */
Object_Instance(Textfield)
/** @endcond */

private:
	struct _settings {
		Font *font;
	};
	static _settings _s;

	char align,showCaret,caretTimer;

public:
	static void setDefaultSettings(Font *f);

	Textfield(int x=0,int y=0,int w=0,int h=0,const char *str=0);

	void setTextAlign(int a) { align = a; }
	bool keyDown(KeyEvent &ke);
	void paint(time_t time);
	void setShowCaret(bool c) { showCaret = c; }
};


}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_TEXTFIELD_H */
