#ifndef _AMANITA_GFX_ICONBUTTON_H
#define _AMANITA_GFX_ICONBUTTON_H

/**
 * @file amanita/gfx/aGfxIconButton.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <amanita/gfx/aGfxComponent.h>

class aGfxImage;
class aGfxFont;

/** A button that uses images instead of a label.
 * @ingroup sdl
 */
class aGfxIconButton : public aGfxComponent,public aGfxMouseListener {
Object_Instance(aGfxIconButton)

private:
	struct _settings {
		aGfxImage *img;
		short plain,active,down,disabled;
	};
	static _settings _s;

	struct iconbutton {
		aGfxImage *img;
		short plain,active,down,disabled;
	} ib;

public:
	aGfxIconButton(int id,int x=0,int y=0);
	aGfxIconButton(aGfxIconButton &b);
	virtual ~aGfxIconButton();

	/**
	 * aGfxImage *img - The image should contain all the images mapped on the same image
	 * uint32_t data[4] - The array should be mapped as follows:
	 *  0 = Default blank Iconbutton, plain
	 *  1 = Default blank Iconbutton, active
	 *  2 = Default blank Iconbutton, down
	 *  3 = Default blank Iconbutton, disabled
	 **/
	static void setDefaultSettings(aGfxImage *img,uint32_t data[4]);

	void setImage(aGfxImage *img,int plain,int active,int down,int disabled);
	void paint(time_t time);
	bool mouseUp(aGfxMouseEvent &me);
};


#endif /* _AMANITA_GFX_ICONBUTTON_H */
