#ifndef _AMANITA_GUI_STATUSBAR_H
#define _AMANITA_GUI_STATUSBAR_H

/**
 * @file amanita/gui/aStatusbar.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#include <stdio.h>
#include <amanita/gui/aWidget.h>


class aStatusbar : public aWidget {
friend class aWindow;
private:

public:
	aStatusbar();
	virtual ~aStatusbar();

	virtual aComponent create();
};

#endif /* _AMANITA_GUI_STATUSBAR_H */

