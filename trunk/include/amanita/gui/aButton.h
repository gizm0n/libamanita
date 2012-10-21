#ifndef _AMANITA_GUI_BUTTON_H
#define _AMANITA_GUI_BUTTON_H

/**
 * @file amanita/gui/aButton.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-13
 * @date Created: 2012-02-13
 */ 


#include <amanita/gui/aWidget.h>


enum {
	aBUTTON_DEFAULT			= 0x00000001,
	aBUTTON_IMAGE				= 0x00000002,
	aBUTTON_CHECKBOX			= 0x00000010,
	aBUTTON_RADIOBUTTON		= 0x00000020,
	aBUTTON_CHECKED			= 0x00000040,
	aBUTTON_LEFT				= 0x00000100,
	aBUTTON_RIGHT				= 0x00000200,
	aBUTTON_CENTER				= 0x00000300,
	aBUTTON_TOP					= 0x00000400,
	aBUTTON_BOTTOM				= 0x00000800,
	aBUTTON_MIDDLE				= 0x00000c00,
	aBUTTON_ALIGNED			= 0x00000f00,
};

enum {
	aBUTTON_CLICKED			= 0x00000001,
	aBUTTON_TOGGLED			= 0x00000002,
};

class aButton : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
Object_Instance(aButton)
/** @endcond */

private:
	aButton *group;

public:	
	aButton(widget_event_handler weh);
	virtual ~aButton();

	virtual void create(aWindow *wnd,uint32_t st);

	void setText(const char *str);
	void setGroup(aButton *g) { group = g; }
	void setChecked(bool s);
	bool isChecked();
	bool isCheckBox() { return type==aWIDGET_CHECKBOX; }
	bool isRadioButton() { return type==aWIDGET_RADIOBUTTON; }
};


#endif /* _AMANITA_GUI_BUTTON_H */

