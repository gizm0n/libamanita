
#include <stdlib.h>
#include <amanita/gui/aSizer.h>


aSizer::aSizer(aWidget *widget,unsigned long st,size_t sz,int x,int y,int w,int h)
		: widget(widget),style(st),sz(sz),children(0),x(x),y(y),width(w),height(h) {
	if(sz) children = (aSizer **)malloc(sizeof(aSizer *)*sz);
}

aSizer::~aSizer() {
	if(children) { free(children);children = 0; }
}


void aSizer::add(aSizer *child) {
}

void aSizer::remove(aSizer *child) {
}

void aSizer::setParent(aSizer *) {
}

void aSizer::makeLayout() {
#if defined(__linux__)

#elif defined(WIN32)
//	HINSTANCE hinst = GetModuleHandle(0);
	
#endif	
}

