#ifndef _AMANITA_TK_SELECT_H
#define _AMANITA_TK_SELECT_H

/**
 * @file amanita/tk/Select.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-13
 */ 


#include <amanita/Vector.h>
#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	SELECT_COMBOBOX		= 0x00000001,
	SELECT_ENTRY			= 0x00000002,
};

enum {
	SELECT_CHANGED			= 0x00000001,
};


/** Select widget.
 * 
 * @ingroup tk */
class Select : public Widget {
friend class Container;
friend class Window;
/** @cond */
Object_Instance(Select)
/** @endcond */

private:
	Vector *items;
	char *item;
	int selected;

public:	
	Select(widget_event_handler weh);
	virtual ~Select();

	virtual void create(Window *wnd,uint32_t st);

	void setItems(const char *arr[]);
	void addItem(const char *str);
	void select(int n);
	int getSelected();
	const char *getItem();
};


}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_SELECT_H */

