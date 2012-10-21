
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/gui/aLabel.h>



Object_Inheritance(aLabel,aWidget)

aLabel::aLabel(widget_event_handler weh) : aWidget(weh,aWIDGET_LABEL) {
}

aLabel::~aLabel() {
}

void aLabel::create(aWindow *wnd,uint32_t st) {
	st = 0;
#ifdef USE_GTK
	component = (aComponent)gtk_label_new(text? text : "");
	if((style&aLABEL_ALIGNED))
		gtk_misc_set_alignment(GTK_MISC(component),
			(style&aLABEL_LEFT)? ((style&aLABEL_CENTER)==aLABEL_CENTER? 0.5 : 0) : ((style&aLABEL_RIGHT)? 1.0 : 0),
			(style&aLABEL_TOP)? ((style&aLABEL_MIDDLE)==aLABEL_MIDDLE? 0.5 : 0) : ((style&aLABEL_BOTTOM)? 1.0 : 0.5));
#endif
#ifdef USE_WIN32
	if((style&aLABEL_ALIGNED)) {
		if((style&aLABEL_LEFT)) {
			if((style&aLABEL_CENTER)==aLABEL_CENTER) st |= SS_CENTER;
			else st |= SS_LEFT;
		} else if((style&aLABEL_RIGHT)) st |= SS_RIGHT;
	}
#endif
	aWidget::create(wnd,st);
}

void aLabel::setText(const char *str) {
	aWidget::setText(str);
	if(component) {
	// If component, change text.
	}
}


