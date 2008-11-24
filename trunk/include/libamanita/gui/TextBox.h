#ifndef _LIBAMANITA_GUI_TEXTBOX_H
#define _LIBAMANITA_GUI_TEXTBOX_H

#include <libamanita/sdl/Font.h>
#include <libamanita/gui/Window.h>


enum {
	TEXTBOX_YES				= 0x00008061,
	TEXTBOX_YES_NO			= 0x00008062,
	TEXTBOX_OK				= 0x00008063,
	TEXTBOX_OK_CANCEL		= 0x00008064,
};


class TextBox : public Window {
RttiObjectInstance(TextBox)

private:
	unsigned int style;

	TextBox(int w,int h,char *msg,unsigned int st,Font *font);

	static void hideMessage(TextBox *m,int id);

public:
	static char yes[16],no[16],ok[16],cancel[16];

	static void showMessage(char *msg,unsigned int st,ActionListener *al,Font *font=0);

	bool keyDown(KeyEvent &ke);
	bool actionPerformed(ActionEvent &ae);
};


#endif /* _LIBAMANITA_GUI_TEXTBOX_H */
