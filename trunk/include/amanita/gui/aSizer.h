#ifndef _AMANITA_GUI_SIZER_H
#define _AMANITA_GUI_SIZER_H

/**
 * @file amanita/gui/aSizer.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <amanita/gui/aWidget.h>


class aSizer {
friend class aWidget;
private:
	aHandle handle;			//!< Handle for the widget or the container of the widget, the actual object that is resized.
	aWidget *widget;			//!< Widget the sizer handles.
	unsigned long style;		//!< Sizer flags for how to handle the layout.
	aSizer *parent;			//!< Parent sizer.
	size_t sz;					//!< Number of children.
	aSizer **children;		//!< Children sizers contained by this sizer.
	int x;						//!< X-position.
	int y;						//!< Y-position.
	int width;					//!< Width.
	int height;					//!< Height.
	int px;						//!< X-padding.
	int py;						//!< Y-padding.

protected:
#if defined(__linux__)
#elif defined(WIN32)
	int getMinWidth();
	int getMinHeight();
#endif

public:
	enum {
		HORIZONTAL					= 0x00000001,
		VERTICAL						= 0x00000002,
		ALIGN_CENTER				= 0x00000010,
		ALIGN_LEFT					= 0x00000020,
		ALIGN_TOP					= 0x00000030,
		ALIGN_RIGHT					= 0x00000040,
		ALIGN_BOTTOM				= 0x00000050,
		STRETCH_NONE				= 0x00000100,
		STRETCH_LEFT				= 0x00001000,
		STRETCH_TOP					= 0x00002000,
		STRETCH_RIGHT				= 0x00004000,
		STRETCH_BOTTOM				= 0x00008000,
		STRETCH_HORIZONTAL		= 0x00005000,
		STRETCH_VERTICAL			= 0x0000a000,
		STRETCH_FULL				= 0x0000f000,
	};


	aSizer(aWidget *widget,unsigned long st,size_t sz=0,int x=0,int y=0,int w=0,int h=0);
	~aSizer();

	void add(aSizer *child);
	void remove(aSizer *child);
	void setParent(aSizer *);
	void makeLayout();
};


#endif /* _AMANITA_GUI_SIZER_H */

