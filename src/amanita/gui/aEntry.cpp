
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/gui/aEntry.h>



aObject_Inheritance(aEntry,aWidget)

aEntry::aEntry(widget_event_handler weh) : aWidget(weh,aWIDGET_ENTRY) {
}

aEntry::~aEntry() {
}

void aEntry::create(aWindow *wnd,uint32_t st) {
#ifdef USE_GTK
	component = (aComponent)gtk_entry_new();
#endif
	aWidget::create(wnd,0);
}

void aEntry::setText(const char *str) {
	aWidget::setText(str);
	// If component, change text.
}


