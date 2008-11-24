#ifndef _LIBAMANITA_GUI_TEXTFIELD_H
#define _LIBAMANITA_GUI_TEXTFIELD_H

#include <libamanita/sdl/Text.h>
#include <libamanita/gui/Component.h>

class Font;

class TextField : public Component,public Text,public KeyListener {
RttiObjectInstance(TextField)

private:
	struct _settings {
		Font *font;
	};
	static _settings _s;

	char align,showCaret,caretTimer;

public:
	static void setDefaultSettings(Font *f);

	TextField(int x=0,int y=0,int w=0,int h=0,char *str=0);

	void setTextAlign(int a) { align = a; }
	bool keyDown(KeyEvent &ke);
	void paint(int time);
	void setShowCaret(bool c) { showCaret = c; }
};



#endif /* _LIBAMANITA_GUI_TEXTFIELD_H */
