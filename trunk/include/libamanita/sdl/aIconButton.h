#ifndef _LIBAMANITA_SDL_AICONBUTTON_H
#define _LIBAMANITA_SDL_AICONBUTTON_H

#include <libamanita/sdl/aComponent.h>

class aImage;
class aFont;

class aIconButton : public aComponent,public aMouseListener {
RttiObjectInstance(aIconButton)

private:
	struct _settings {
		aImage *img;
		short plain,active,down,disabled;
	};
	static _settings _s;

	struct iconbutton {
		aImage *img;
		short plain,active,down,disabled;
	} ib;

public:
	aIconButton(int id,int x=0,int y=0);
	aIconButton(aIconButton &b);
	virtual ~aIconButton();

	/**
	 * aImage *img - The image should contain all the images mapped on the same image
	 * Uint32 data[4] - The array should be mapped as follows:
	 *  0 = Default blank Iconbutton, plain
	 *  1 = Default blank Iconbutton, active
	 *  2 = Default blank Iconbutton, down
	 *  3 = Default blank Iconbutton, disabled
	 **/
	static void setDefaultSettings(aImage *img,Uint32 data[4]);

	void setImage(aImage *img,int plain,int active,int down,int disabled);
	void paint(time_t time);
	bool mouseUp(aMouseEvent &me);
};


#endif /* _LIBAMANITA_SDL_AICONBUTTON_H */
