#ifndef _AMANITA_GUI_ICON_H
#define _AMANITA_GUI_ICON_H

/**
 * @file amanita/gui/Icon.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/gui/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Image;
class Font;
/** @endcond */


/** A button that uses images instead of a label.
 * 
 * @ingroup gui */
class Icon : public Widget,public MouseListener {
/** @cond */
Object_Instance(Icon)
/** @endcond */

private:
	struct _settings {
		Image *img;
		short plain,active,down,disabled;
	};
	static _settings _s;

	struct iconbutton {
		Image *img;
		short plain,active,down,disabled;
	} ib;

public:
	Icon(int id,int x=0,int y=0);
	Icon(Icon &b);
	virtual ~Icon();

	/**
	 * Image *img - The image should contain all the images mapped on the same image
	 * uint32_t data[4] - The array should be mapped as follows:
	 *  0 = Default blank Iconbutton, plain
	 *  1 = Default blank Iconbutton, active
	 *  2 = Default blank Iconbutton, down
	 *  3 = Default blank Iconbutton, disabled
	 **/
	static void setDefaultSettings(Image *img,uint32_t data[4]);

	void setImage(Image *img,int plain,int active,int down,int disabled);
	void paint(time_t time);
	bool mouseUp(MouseEvent &me);
};

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_ICON_H */
