
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#endif
#include <amanita/gui/aText.h>


aObject_Inheritance(aText,aWidget)

aText::aText(widget_event_handler weh) : aWidget(weh,aWIDGET_ENTRY) {
}

aText::~aText() {
}

void aText::create(aWindow *wnd,uint32_t st) {
	if((style&aENTRY_MULTILINE)) type = aWIDGET_TEXT;
#ifdef USE_GTK
	if(type==aWIDGET_TEXT) {
		component = (aComponent)gtk_text_view_new();
	} else {
		component = (aComponent)gtk_entry_new();
	}
#endif
	aWidget::create(wnd,0);
}

void aText::setText(const char *str) {
	aWidget::setText(str);
	// If component, change text.
}


