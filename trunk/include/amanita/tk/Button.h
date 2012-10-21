#ifndef _AMANITA_TK_BUTTON_H
#define _AMANITA_TK_BUTTON_H

/**
 * @file amanita/tk/Button.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-13
 */ 


#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	BUTTON_DEFAULT				= 0x00000001,
	BUTTON_IMAGE				= 0x00000002,
	BUTTON_CHECKBOX			= 0x00000010,
	BUTTON_RADIOBUTTON		= 0x00000020,
	BUTTON_CHECKED				= 0x00000040,
	BUTTON_LEFT					= 0x00000100,
	BUTTON_RIGHT				= 0x00000200,
	BUTTON_CENTER				= 0x00000300,
	BUTTON_TOP					= 0x00000400,
	BUTTON_BOTTOM				= 0x00000800,
	BUTTON_MIDDLE				= 0x00000c00,
	BUTTON_ALIGNED				= 0x00000f00,
};

enum {
	BUTTON_CLICKED				= 0x00000001,
	BUTTON_TOGGLED				= 0x00000002,
};


/** Button widget.
 * 
 * @ingroup tk */
class Button : public Widget {
friend class Container;
friend class Window;
/** @cond */
Object_Instance(Button)
/** @endcond */

private:
	Button *group;

public:	
	Button(widget_event_handler weh);
	virtual ~Button();

	virtual void create(Window *wnd,uint32_t st);

	void setText(const char *str);
	void setGroup(Button *g) { group = g; }
	void setChecked(bool s);
	bool isChecked();
	bool isCheckBox() { return type==WIDGET_CHECKBOX; }
	bool isRadioButton() { return type==WIDGET_RADIOBUTTON; }
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_BUTTON_H */

