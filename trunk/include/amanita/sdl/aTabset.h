#ifndef _LIBAMANITA_SDL_TABSET_H
#define _LIBAMANITA_SDL_TABSET_H

/**
 * @file libamanita/sdl/aTabset.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/sdl/aComponent.h>


enum {
	TABSET_TOP			= 0,
	TABSET_BOTTOM		= 1,
};

class aImage;
class aFont;

class aTabset : public aComponent,public aMouseListener {
aObject_Instance(aTabset)

private:
	struct _settings {
		aImage *img;
		aFont *font1,*font2;
		struct button {
			SDL_Rect *l1,*l2,*c1,*c2,*c3,*r1,*r2,*b1,*b2;
			SDL_Rect ins;
		} top,bottom;
	};
	static _settings _s;

protected:
	struct tab {
		char *text;
		int index;
		int tw1,tw2;
		SDL_Rect r;
	};
	int style,ntabs,act;
	tab *tabs;
	_settings::button *t;
	aActionEvent ae;

	void updateTabs();
	void deleteTabs();

public:
	aTabset(int id,int x=0,int y=0,int st=0);
	virtual ~aTabset();

	/**
	 * aImage *img - The image should contain all the scrollimages mapped on the same image
	 * Uint32 data[10] - The array should be mapped as follows:
	 *  0 = Top tabset, Inactive, left edge of first tab
	 *  1 = Top tabset, Active, left edge
	 *  2 = Top tabset, Inactive to inactive, section between two tabs
	 *  3 = Top tabset, Inctive to active, section between two tabs
	 *  4 = Top tabset, Active to inactive, section between two tabs
	 *  5 = Top tabset, Inactive, right edge o last tab
	 *  6 = Top tabset, Active, right edge o last tab
	 *  7 = Top tabset, Inactive tab bar
	 *  8 = Top tabset, Active tab bar
	 *  9 = Top tabset, Insets, Left
	 * 10 = Top tabset, Insets, Top
	 * 11 = Top tabset, Insets, Right
	 * 12 = Top tabset, Insets, Bottom
	 * 13 = Bottom tabset, Inactive, left edge of first tab
	 * 14 = Bottom tabset, Active, left edge
	 * 15 = Bottom tabset, Inactive to inactive, section between two tabs
	 * 16 = Bottom tabset, Inctive to active, section between two tabs
	 * 17 = Bottom tabset, Active to inactive, section between two tabs
	 * 18 = Bottom tabset, Inactive, right edge o last tab
	 * 19 = Bottom tabset, Active, right edge o last tab
	 * 20 = Bottom tabset, Inactive tab bar
	 * 21 = Bottom tabset, Active tab bar
	 * 22 = Bottom tabset, Insets, Left
	 * 23 = Bottom tabset, Insets, Top
	 * 24 = Bottom tabset, Insets, Right
	 * 25 = Bottom tabset, Insets, Bottom
	 **/
	static void setDefaultSettings(aImage *img,aFont *f1,aFont *f2,Uint32 data[26]);

	void setStyle(int st=0);
	void setTabs(const char *str[],int len);
	void setActiveTab(int a);
	int getActiveTab() { return act; }
	int getPreviousTab(int n=0);
	bool mouseDown(aMouseEvent &me);
	void paint(time_t time);
};



#endif /* _LIBAMANITA_SDL_TABSET_H */
