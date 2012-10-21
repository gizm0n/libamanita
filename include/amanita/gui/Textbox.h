#ifndef _AMANITA_GUI_TEXTBOX_H
#define _AMANITA_GUI_TEXTBOX_H

/**
 * @file amanita/gui/Textbox.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/gui/Font.h>
#include <amanita/gui/Window.h>


/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


enum {
	TEXTBOX_YES				= 0x00008061,
	TEXTBOX_YES_NO			= 0x00008062,
	TEXTBOX_OK				= 0x00008063,
	TEXTBOX_OK_CANCEL		= 0x00008064,
};


/** The Textbox is a modal dialog that shows a message.
 * 
 * @ingroup gui */
class Textbox : public Window {
/** @cond */
Object_Instance(Textbox)
/** @endcond */

private:
	unsigned int style;

	Textbox(int w,int h,const char *msg,unsigned int st,Font *font);

	static void hideMessage(Textbox *m,int id);

public:
	static char yes[16],no[16],ok[16],cancel[16];

	static void showMessage(const char *msg,unsigned int st,ActionListener *al,Font *font=0);

	bool keyDown(KeyEvent &ke);
	bool actionPerformed(ActionEvent &ae);
};

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_TEXTBOX_H */
