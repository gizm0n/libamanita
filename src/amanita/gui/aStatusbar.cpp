
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <amanita/gui/aStatusbar.h>


aStatusbar::aStatusbar() : aWidget(0,WIDGET_STATUSBAR) {
}


aStatusbar::~aStatusbar() {
}

aComponent aStatusbar::create() {
#ifdef __linux__
	container = (aComponent)gtk_statusbar_new();
	component = container;
#endif
	return container;
}




