#ifndef _LIBAMANITA_SDL_ABUTTON_H
#define _LIBAMANITA_SDL_ABUTTON_H

#include <libamanita/sdl/aComponent.h>

enum {
	COM_ID_CLOSE			= 0x00008021,		// Close aButton ID, for aWindow class
	COM_ID_UP				= 0x00008031,		// Up aButton, for aScroll
	COM_ID_DOWN				= 0x00008032,		// Down aButton, for aScroll
	COM_ID_LEFT				= 0x00008033,		// Left aButton, for aScroll
	COM_ID_RIGHT			= 0x00008034,		// Right aButton, for aScroll
	COM_ID_YES				= 0x00008051,		// Yes aButton,for aTextbox
	COM_ID_NO				= 0x00008052,		// No aButton,for aTextbox
	COM_ID_OK				= 0x00008053,		// OK aButton,for aTextbox
	COM_ID_CANCEL			= 0x00008054,		// Cancel aButton,for aTextbox
};

class aImage;
class aFont;

class aButton : public aComponent,public aMouseListener {
RttiObjectInstance(aButton)

private:
	struct _settings {
		aImage *img;
		aFont *font;
		struct state {
			SDL_Rect *tl,*t,*tr,*l,*r,*bl,*b,*br,*c;
		} st[4];
	};
	static _settings _s;

	struct button {
		struct icon {
			aImage *img;
			short index;
		} ic;
		char *text,align;		// aText and textalign
		short tw,tx,ty;		// aText width and text coordinates
		aFont *font;				// aText font
		SDL_Rect ins;			// aText insets
	} bt;

public:
	aButton(int id,int x=0,int y=0,int w=0,int h=0);
	aButton(aButton &b);
	virtual ~aButton();

	/**
	 * aImage *img - The image should contain all the buttonimages mapped on the same image
	 * aFont *f - Default font used for buttontext
	 * uint32_t data[35] - The array should be mapped as follows:
	 *  0 = Top left corner, plain
	 *  1 = Top border, plain
	 *  2 = Top right corner, plain
	 *  3 = Left border, plain
	 *  4 = Right border, plain
	 *  5 = Bottom left corner, plain
	 *  6 = Bottom border, plain
	 *  7 = Bottom right corner, plain
	 *  8 = Center Background, plain
	 *  9 = Top left corner, active
	 * 10 = Top border, active
	 * 11 = Top right corner, active
	 * 12 = Left border, active
	 * 13 = Right border, active
	 * 14 = Bottom left corner, active
	 * 15 = Bottom border, active
	 * 16 = Bottom right corner, active
	 * 17 = Center Background, active
	 * 18 = Top left corner, down
	 * 19 = Top border, down
	 * 20 = Top right corner, down
	 * 21 = Left border, down
	 * 22 = Right border, down
	 * 23 = Bottom left corner, down
	 * 24 = Bottom border, down
	 * 25 = Bottom right corner, down
	 * 26 = Center Background, down
	 * 27 = Top left corner, disabled
	 * 28 = Top border, disabled
	 * 29 = Top right corner, disabled
	 * 30 = Left border, disabled
	 * 31 = Right border, disabled
	 * 32 = Bottom left corner, disabled
	 * 33 = Bottom border, disabled
	 * 34 = Bottom right corner, disabled
	 * 35 = Center Background, disabled
	 */
	static void setDefaultSettings(aImage *img,aFont *f,uint32_t data[36]);

	void setIcon(aImage *i,int n=0) { bt.ic.img = i,bt.ic.index = n; }
	void setText(const char *text);
	const char *getText() { return bt.text; }
	void setFont(aFont *font);
	void setTextAlign(int align);
	void setInsets(int l,int t,int r,int b);
	void paint(time_t time);
	bool mouseUp(aMouseEvent &me);
};


#endif /* _LIBAMANITA_SDL_ABUTTON_H */
