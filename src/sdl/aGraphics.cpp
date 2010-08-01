
#include "../config.h"
#include <amanita/aMath.h>
#include <amanita/sdl/aGraphics.h>
#include <amanita/sdl/aFont.h>


aGraphics g;


void aGraphics::initSDL(const char *caption,const char *ic1,SDL_Surface *ic2,int w,int h,int d,int f) {
	if(ic1 || ic2) {
		if(ic1) {
			ic2 = SDL_LoadBMP(ic1);
//			SDL_SetColorKey(ic2,SDL_SRCCOLORKEY,SDL_MapRGB(ic2->format,0xff,0,0xff));
		}
		SDL_WM_SetIcon(ic2,0);
	}
	g.screen = SDL_SetVideoMode(w,h,d,f);
	if(!g.screen) {
		fprintf(stderr,"Unable to set %dx%d video: %s\n",w,h,SDL_GetError());
		fflush(stderr);
		exit(0);
	}
	if(caption) setCaption(caption,caption);
	g.canvas = g.screen;
	if(ic1) g.icon = ic2;
	g.resetClip();
}

void aGraphics::close() {
	g.screen = 0;
	g.canvas = 0;
	if(g.icon) { SDL_FreeSurface(g.icon);g.icon = 0; }
	g.font = 0;
}


void aGraphics::pixel(int x,int y,uint32_t c) {
	int bpp = canvas->format->BytesPerPixel;
	char *p = ((char *)canvas->pixels)+x*bpp+y*canvas->pitch,*cp = (char *)&c;
	for(; bpp; bpp--) *p++ = *cp++;
}

void aGraphics::draw(int x,int y,SDL_Surface *s,SDL_Rect *src) {
	SDL_Rect dst = { x,y,0,0 };
	SDL_BlitSurface(s,src,canvas,&dst);
}

// Bresenhem's line algorithm
void aGraphics::drawLine(int x1,int y1,int x2,int y2,uint32_t c) {
	int pitch = canvas->pitch,bpp = canvas->format->BytesPerPixel;
	char *pixels = (char *)canvas->pixels,*p,*cp;
	int xinc1,xinc2,yinc1,yinc2,den,num,numadd,npx,cpx;
	int dx = abs(x2-x1),dy = abs(y2-y1);
	int i,x = x1*bpp,y = y1*pitch;
	int l = cl.x*bpp,t = cl.y*pitch,r = (cl.x+cl.w)*bpp,b = (cl.y+cl.h)*pitch;
	if(x2>=x1) xinc1 = xinc2 = bpp;else xinc1 = xinc2 = -bpp;
	if(y2>=y1) yinc1 = yinc2 = pitch;else yinc1 = yinc2 = -pitch;
	if(dx>=dy) xinc1 = yinc2 = 0,den = dx,num = dx>>1,numadd = dy,npx = dx;
	else xinc2 = yinc1 = 0,den = dy,num = dy>>1,numadd = dx,npx = dy;
	for(cpx=0; cpx<=npx; cpx++) {
		if(x>=l && x<r && y>=t && y<b) {
			p = pixels+x+y,cp = (char *)&c;
			for(i=0; i<bpp; i++) p[i] = cp[i];
		}
		num += numadd;
		if(num>=den) num -= den,x += xinc1,y += yinc1;
		x += xinc2;y += yinc2;
	}
}

void aGraphics::drawDottedRect(int x,int y,int w,int h,uint32_t *p) {
	if(x+w<=cl.x || x>=cl.x+cl.w || y+h<=cl.y || y>=cl.y+cl.h) return;
	int pitch = canvas->pitch,bpp = canvas->format->BytesPerPixel;
	char *pixels = (char *)canvas->pixels,*cp;
	int i,j,n,p1 = p[0],p2 = p[1];
	char *pl = pixels+x*bpp,*pt = pixels+y*pitch,*pr = pixels+(x+w-1)*bpp,*pb = pixels+(y+h-1)*pitch;
	if(x<cl.x) for(i=y,n=y+h; i<n; i++,pl+=pitch)
		for(j=0,cp=(char *)&p[2+(i+p1)%p2]; j<bpp; j++) pl[j] = cp[j];
	if(y<cl.y) for(i=x+1,n=x+w-1; i<n; i++)
		for(j=0,cp=(char *)&p[2+(i+p1)%p2]; j<bpp; j++) *pt++ = cp[j];
	if(x+w<=cl.x+cl.w) for(i=y,n=y+h; i<n; i++,pr+=pitch)
		for(j=0,cp=(char *)&p[2+(i+p1)%p2]; j<bpp; j++) pr[j] = cp[j];
	if(y+h<=cl.y+cl.h) for(i=x+1,n=x+w-1; i<n; i++)
		for(j=0,cp=(char *)&p[2+(i+p1)%p2]; j<bpp; j++) *pb++ = cp[j];
}



void aGraphics::fillRect(int x,int y,int w,int h,uint32_t c) {
	SDL_Rect r = (SDL_Rect){x,y,w,h};
	SDL_FillRect(canvas,&r,c);
}


bool aGraphics::screenDump(const char *fn) {
	return aImage::save(fn,screen);
}


