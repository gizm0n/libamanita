#ifndef _AMANITA_GUI_IMAGE_H
#define _AMANITA_GUI_IMAGE_H

/**
 * @file amanita/gui/Image.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <string.h>
#include <stdio.h>
#ifdef USE_DD
#include <windows.h>
#include <ddraw.h>
#include <olectl.h> 
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#include <amanita/Geometry.h>
#include <amanita/Object.h>


/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


#ifdef USE_SDL
typedef SDL_Surface *Surface;
#endif
#ifdef USE_DD
typedef LPDIRECTDRAWSURFACE7 Surface;
#endif


/** A generic image class, for loading and handling images.
 * 
 * @ingroup gui */
class Image : public Object {
friend class Graphics;
/** @cond */
Object_Instance(Image)
/** @endcond */

protected:
	char *name,*file;
	Surface surface;
	rect16_t *map;
	unsigned int mapSize,mapRow;

	void load(const char *fn);
	void draw(rect16_t &src,rect16_t &dst);

public:
	static int created,deleted;

	static Image **readXIM(const char *p,const char *fn,int &n);
	static Image **parseXIM(const char *p,const char *lines[],int &n);
	static void formatString(char *str,const char *p,const char *format,int num);

	static bool save(const char *fn,Surface s);
	bool save(const char *fn) { return save(fn,surface); }

	Image();
	Image(int w,int h);
	Image(const char *fn);
	~Image();

	void createMap(int sz);
	void createMap(int rw,int rh);
	void createMap(rect16_t *m,int l);

	void draw(int x,int y) { draw(x,y,map[0]); }
	void draw(int x,int y,int i) { draw(x,y,map[i]); }
	void draw(int i) { rect16_t *r = &map[i];draw(r->x,r->y,*r); }
	void draw(int x,int y,rect16_t *src) { draw(x,y,*src); }
	void draw(int x,int y,rect16_t &src);

	void draw(int x,int y,int w,int h) { draw(x,y,w,h,map[0]); }
	void draw(int x,int y,int w,int h,int i) { draw(x,y,w,h,map[i]); }
	void draw(int x,int y,int w,int h,rect16_t *src) { draw(x,y,w,h,*src); }
	void draw(int x,int y,int w,int h,rect16_t &src);

	const char *getName() { return name; }
	const char *getFileName() { return file; }

	int cells() { return mapSize-1; }
	rect16_t *getCell(int i) { return &map[i]; }
	rect16_t *getCell(int x,int y) { return &map[1+x+y*mapRow]; }
	void setCell(int index,int x,int y,int w,int h);
	Surface getSurface() { return surface; }
	int getWidth() { return map[0].w; }
	int getHeight() { return map[0].h; }
	int getWidth(int i) { return i>=0 && (unsigned int)i<mapSize? map[i].w : 0; }
	int getHeight(int i) { return i>=0 && (unsigned int)i<mapSize? map[i].h : 0; }
};


}; /* namespace gui */
}; /* namespace a */



#endif /* _AMANITA_GUI_IMAGE_H */

