#ifndef _AMANITA_TK_LABEL_H
#define _AMANITA_TK_LABEL_H

/**
 * @file amanita/tk/Label.h  
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
	LABEL_LEFT					= 0x00000001,
	LABEL_RIGHT					= 0x00000002,
	LABEL_CENTER				= 0x00000003,
	LABEL_TOP					= TOP,
	LABEL_BOTTOM				= BOTTOM,
	LABEL_MIDDLE				= MIDDLE,
	LABEL_ALIGNED				= 0x0000000f,
};


/** Label widget.
 * 
 * @ingroup tk */
class Label : public Widget {
friend class Container;
friend class Window;
/** @cond */
Object_Instance(Label)
/** @endcond */

public:	
	Label(widget_event_handler weh);
	virtual ~Label();

	virtual void create(Window *wnd,uint32_t st);

	void setText(const char *str);
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_LABEL_H */

