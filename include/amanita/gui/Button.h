#ifndef _AMANITA_GUI_BUTTON_H
#define _AMANITA_GUI_BUTTON_H

/**
 * @file amanita/gui/Button.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */

#include <amanita/gui/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


enum {
	COM_ID_CLOSE			= 0x00008021,		//!< Close button ID, for Window class
	COM_ID_UP				= 0x00008031,		//!< Up button, for Scroll
	COM_ID_DOWN				= 0x00008032,		//!< Down button, for Scroll
	COM_ID_LEFT				= 0x00008033,		//!< Left button, for Scroll
	COM_ID_RIGHT			= 0x00008034,		//!< Right button, for Scroll
	COM_ID_YES				= 0x00008051,		//!< Yes button,for Textbox
	COM_ID_NO				= 0x00008052,		//!< No button,for Textbox
	COM_ID_OK				= 0x00008053,		//!< OK button,for Textbox
	COM_ID_CANCEL			= 0x00008054,		//!< Cancel button,for Textbox
};


/** @cond */
class Image;
class Font;
/** @endcond */


/** Button.
 * 
 * @ingroup gui */
class Button : public Widget,public MouseListener {
/** @cond */
Object_Instance(Button)
/** @endcond */

private:
	struct _settings {
		Image *img;
		Font *font;
		struct state {
			rect16_t *tl,*t,*tr,*l,*r,*bl,*b,*br,*c;
		} st[4];
	};
	static _settings _s;

	struct button {
		struct icon {
			Image *img;
			short index;
		} ic;
		char *text;					//!< Button text
		char align;					//!< Button align
		short tw;					//!< Text width
		short tx;					//!< X-coordinate of text
		short ty;					//!< Y-coordinate of text
		Font *font;					//!< Button text font
		rect16_t ins;				//!< Button insets
	} bt;

public:
	Button(int id,int x=0,int y=0,int w=0,int h=0);
	Button(Button &b);
	virtual ~Button();

	/**
	 * @param img The image should contain all the buttonimages mapped on the same image
	 * @param f Default font used for buttontext
	 * @param data The array should be mapped as follows:
	 *    - 0. Top left corner, plain
	 *    - 1. Top border, plain
	 *    - 2. Top right corner, plain
	 *    - 3. Left border, plain
	 *    - 4. Right border, plain
	 *    - 5. Bottom left corner, plain
	 *    - 6. Bottom border, plain
	 *    - 7. Bottom right corner, plain
	 *    - 8. Center Background, plain
	 *    - 9. Top left corner, active
	 *    - 10. Top border, active
	 *    - 11. Top right corner, active
	 *    - 12. Left border, active
	 *    - 13. Right border, active
	 *    - 14. Bottom left corner, active
	 *    - 15. Bottom border, active
	 *    - 16. Bottom right corner, active
	 *    - 17. Center Background, active
	 *    - 18. Top left corner, down
	 *    - 19. Top border, down
	 *    - 20. Top right corner, down
	 *    - 21. Left border, down
	 *    - 22. Right border, down
	 *    - 23. Bottom left corner, down
	 *    - 24. Bottom border, down
	 *    - 25. Bottom right corner, down
	 *    - 26. Center Background, down
	 *    - 27. Top left corner, disabled
	 *    - 28. Top border, disabled
	 *    - 29. Top right corner, disabled
	 *    - 30. Left border, disabled
	 *    - 31. Right border, disabled
	 *    - 32. Bottom left corner, disabled
	 *    - 33. Bottom border, disabled
	 *    - 34. Bottom right corner, disabled
	 *    - 35. Center Background, disabled
	 */
	static void setDefaultSettings(Image *img,Font *f,uint32_t data[36]);

	void setIcon(Image *i,int n=0) { bt.ic.img = i,bt.ic.index = n; }
	void setText(const char *text);
	const char *getText() { return bt.text; }
	void setFont(Font *font);
	void setTextAlign(int align);
	void setInsets(int l,int t,int r,int b);
	void paint(time_t time);
	bool mouseUp(MouseEvent &me);
};

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_BUTTON_H */
