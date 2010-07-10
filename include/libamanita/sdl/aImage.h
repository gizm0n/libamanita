#ifndef _LIBAMANITA_SDL_IMAGE_H
#define _LIBAMANITA_SDL_IMAGE_H

/**
 * @file libamanita/sdl/aImage.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <string.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <libamanita/aObject.h>



class aImage : public aObject {
friend class aGraphics;

aObject_Instance(aImage)

protected:
	char *name,*file;
	SDL_Surface *surface;
	SDL_Rect *map;
	unsigned int mapSize,mapRow;

	void load(const char *fn);
	void draw(SDL_Rect &src,SDL_Rect &dst);

public:
	static int created,deleted;

	static aImage **readXIM(const char *p,const char *fn,int &n);
	static aImage **parseXIM(const char *p,const char *lines[],int &n);
	static void formatString(char *str,const char *p,const char *format,int num);

	static bool save(const char *fn,SDL_Surface *s);
	bool save(const char *fn) { return save(fn,surface); }

	aImage();
	aImage(int w,int h);
	aImage(const char *fn);
	~aImage();

	void createMap(int sz);
	void createMap(int rw,int rh);
	void createMap(SDL_Rect *m,int l);

	void draw(int x,int y) { draw(x,y,map[0]); }
	void draw(int x,int y,int i) { draw(x,y,map[i]); }
	void draw(int i) { SDL_Rect *r = &map[i];draw(r->x,r->y,*r); }
	void draw(int x,int y,SDL_Rect *src) { draw(x,y,*src); }
	void draw(int x,int y,SDL_Rect &src);

	void draw(int x,int y,int w,int h) { draw(x,y,w,h,map[0]); }
	void draw(int x,int y,int w,int h,int i) { draw(x,y,w,h,map[i]); }
	void draw(int x,int y,int w,int h,SDL_Rect *src) { draw(x,y,w,h,*src); }
	void draw(int x,int y,int w,int h,SDL_Rect &src);

	const char *getName() { return name; }
	const char *getFileName() { return file; }

	int cells() { return mapSize-1; }
	SDL_Rect *getCell(int i) { return &map[i]; }
	SDL_Rect *getCell(int x,int y) { return &map[1+x+y*mapRow]; }
	void setCell(int index,int x,int y,int w,int h);
	SDL_Surface *getSurface() { return surface; }
	int getWidth() { return map[0].w; }
	int getHeight() { return map[0].h; }
	int getWidth(int i) { return i>=0 && (unsigned int)i<mapSize? map[i].w : 0; }
	int getHeight(int i) { return i>=0 && (unsigned int)i<mapSize? map[i].h : 0; }
};





#endif /* _LIBAMANITA_SDL_IMAGE_H */

