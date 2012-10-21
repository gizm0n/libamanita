
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/tk/Label.h>



namespace a {
namespace tk {


Object_Inheritance(Label,Widget)

Label::Label(widget_event_handler weh) : Widget(weh,WIDGET_LABEL) {
}

Label::~Label() {
}

void Label::create(Window *wnd,uint32_t st) {
	st = 0;
#ifdef USE_GTK
	component = (Component)gtk_label_new(text? text : "");
	if((style&LABEL_ALIGNED))
		gtk_misc_set_alignment(GTK_MISC(component),
			(style&LABEL_LEFT)? ((style&LABEL_CENTER)==LABEL_CENTER? 0.5 : 0) : ((style&LABEL_RIGHT)? 1.0 : 0),
			(style&LABEL_TOP)? ((style&LABEL_MIDDLE)==LABEL_MIDDLE? 0.5 : 0) : ((style&LABEL_BOTTOM)? 1.0 : 0.5));
#endif
#ifdef USE_WIN32
	if((style&LABEL_ALIGNED)) {
		if((style&LABEL_LEFT)) {
			if((style&LABEL_CENTER)==LABEL_CENTER) st |= SS_CENTER;
			else st |= SS_LEFT;
		} else if((style&LABEL_RIGHT)) st |= SS_RIGHT;
	}
#endif
	Widget::create(wnd,st);
}

void Label::setText(const char *str) {
	Widget::setText(str);
	if(component) {
	// If component, change text.
	}
}


}; /* namespace tk */
}; /* namespace a */


