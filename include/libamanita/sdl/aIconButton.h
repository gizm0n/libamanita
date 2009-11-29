#ifndef _LIBAMANITA_GUI_ICONBUTTON_H
#define _LIBAMANITA_GUI_ICONBUTTON_H

#include <libamanita/gui/Component.h>

class Image;
class Font;

class IconButton : public Component,public MouseListener {
RttiObjectInstance(IconButton)

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
	IconButton(int id,int x=0,int y=0);
	IconButton(IconButton &b);
	virtual ~IconButton();

	/**
	 * Image *img - The image should contain all the images mapped on the same image
	 * Uint32 data[4] - The array should be mapped as follows:
	 *  0 = Default blank Iconbutton, plain
	 *  1 = Default blank Iconbutton, active
	 *  2 = Default blank Iconbutton, down
	 *  3 = Default blank Iconbutton, disabled
	 **/
	static void setDefaultSettings(Image *img,Uint32 data[4]);

	void setImage(Image *img,int plain,int active,int down,int disabled);
	void paint(time_t time);
	bool mouseUp(MouseEvent &me);
};


#endif /* _LIBAMANITA_GUI_ICONBUTTON_H */
