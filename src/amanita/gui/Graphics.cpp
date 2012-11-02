
#include "../_config.h"
#ifdef USE_DD
#include <windows.h>
#include <ddraw.h>
#include <olectl.h> 
#include <mmsystem.h> 
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#ifdef HAVE_SDL_IMG
#include <SDL/SDL_image.h>
#endif
#ifdef HAVE_SDL_TTF
#include <SDL/SDL_ttf.h>
#endif
#endif
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>
#include <amanita/gui/Font.h>


namespace a {
namespace gui {


Graphics g;


Graphics::Graphics() {
#ifdef USE_DD
	front = 0;
	back = 0;
#endif
#ifdef USE_SDL
	screen = 0;
	icon = 0;
#endif
	canvas = 0;
	font = 0;
}

Graphics::~Graphics() {
}

int Graphics::getWidth() {
#ifdef USE_DD
	return width;
#endif
#ifdef USE_SDL
	return canvas->w;
#endif
}

int Graphics::getHeight() {
#ifdef USE_DD
	return width;
#endif
#ifdef USE_SDL
	return canvas->h;
#endif
}

void Graphics::init(Application &a) {
	app = &a;
#ifdef USE_DD
	LPDIRECTDRAW lpdd = 0;
	HRESULT ret;
	int coop = app->isFullscreen()? DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|DDSCL_ALLOWREBOOT : DDSCL_NORMAL;
	if((ret=DirectDrawCreate(NULL,&lpdd,NULL))!=DD_OK)
		MessageBox(hwnd,_T("Failed to create the DirectDraw object"),_T("Error"),MB_OK);
	else if((ret=lpdd->QueryInterface(IID_IDirectDraw7,(LPVOID*)&dd))!=DD_OK)
		MessageBox(hwnd,_T("Failed to query interface for DirectDraw7"),_T("Error"),MB_OK);
	else if((ret=dd->SetCooperativeLevel(hwnd,coop))!=DD_OK)
		MessageBox(hwnd,_T("Failed to set DirectDraw cooperative level"),_T("Error"),MB_OK);
	if(lpdd) lpdd->Release();
#endif
}

#ifdef USE_SDL
void Graphics::setXPMIcon(const char **xpm) {
#ifdef HAVE_SDL_IMG
	icon = IMG_ReadXPMFromArray((char **)xpm);
//	SDL_SetColorKey(icon,SDL_SRCCOLORKEY,SDL_MapRGB(icon->format,0xff,0,0xff));
	SDL_WM_SetIcon(icon,0);
#endif
}

void Graphics::setPNGIcon(const char *png) {
#ifdef HAVE_SDL_IMG
debug_output("Graphics::setPNGIcon[%s]\n",png);
	SDL_RWops *rwop = SDL_RWFromFile(png,"rb");
	icon = IMG_LoadPNG_RW(rwop);
//	SDL_SetColorKey(icon,SDL_SRCCOLORKEY,SDL_MapRGB(icon->format,0xff,0,0xff));
	SDL_WM_SetIcon(icon,0);
#endif
}
#endif /* USE_SDL */

void Graphics::openDisplay(int w,int h,int d) {
#ifdef USE_DD
	HRESULT ret;
	if(w>0 && h>0 && d>0 &&
			(ret=dd->SetDisplayMode(w,h,d,0,0))!=DD_OK) {
		MessageBox(hwnd,_T("Failed to set display mode"),_T("Error"),MB_OK);
	} else {
		eraseDesc();
		dd->GetDisplayMode(&ddsd);
		width = ddsd.dwWidth;
		height = ddsd.dwHeight;
		bpp = ddsd.ddpfPixelFormat.dwRGBBitCount/8;

		if(back) back->Release();
		if(front) front->Release();
		eraseDesc();
		if(app->isFullscreen()) {
			ddsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
			ddsd.dwBackBufferCount = 1;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
		} else {
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		}
		if(FAILED(dd->CreateSurface(&ddsd,&front,NULL)))
			MessageBox(hwnd,_T("Failed to create the primary surface"),_T("Error"),MB_OK);
		else {
			if(app->isFullscreen()) {
				ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
				if(FAILED(front->GetAttachedSurface(&ddsd.ddsCaps,&back)))
					MessageBox(hwnd,_T("Failed to create the back surface"),_T("Error"),MB_OK);
			} else {
				RECT rect;
				GetClientRect(hwnd,&rect);
				if((dd->CreateClipper(0,&lpclip,NULL))==DD_OK) {
					if((lpclip->SetHWnd(0,hwnd))!=DD_OK)
						MessageBox(hwnd,_T("SetHWnd on primary clip failed"),_T("Error"),MB_OK);
					if((front->SetClipper(lpclip))!=DD_OK)
						MessageBox(hwnd,_T("SetClipper failed"),_T("Error"),MB_OK);
				} else
					MessageBox(hwnd,_T("CreateClipper failed"),_T("Error"),MB_OK);
				eraseDesc();
				ddsd.dwFlags = DDSD_WIDTH|DDSD_HEIGHT;
				ddsd.dwWidth = width = rect.right-rect.left;
				ddsd.dwHeight = height = rect.bottom-rect.top;
				if(FAILED(dd->CreateSurface(&ddsd,&back,NULL)))
					MessageBox(hwnd,_T("Failed to create the back surface"),_T("Error"),MB_OK);
			}
			canvas = back;
//			fontEngine = new FontEngine(dd);
			font = 0;
		}
	}
	canvas = back;
	hdc = CreateCompatibleDC(NULL);
#endif
#ifdef USE_SDL
	if(app->useSDL()) {
		screen = SDL_SetVideoMode(w,h,d,SDL_SWSURFACE|SDL_ANYFORMAT);
		if(!screen) {
			fprintf(stderr,"Unable to set %dx%d video: %s\n",w,h,SDL_GetError());
			fflush(stderr);
//			return APP_SDL;
		}
	}
	canvas = screen;
#endif
	resetClip();
}

void Graphics::close() {
#ifdef USE_DD
	DeleteDC(hdc);
	if(back) back->Release();
	if(front) front->Release();
	if(dd) dd->Release();
//	if(fontEngine) delete fontEngine;
	back = 0,front = 0,dd = 0,fontEngine = 0;
#endif
#ifdef USE_SDL
	screen = 0;
	if(icon) SDL_FreeSurface(icon);
	icon = 0;
#endif
	canvas = 0;
}

#ifdef USE_DD
void Graphics::setHWND(HWND w) {
	hwnd = w;
}

void Graphics::eraseDesc() {
	memset(&ddsd,0,sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
}
#endif

void Graphics::resetCanvas() {
#ifdef USE_DD
	canvas = back;
#endif
#ifdef USE_SDL
	canvas = screen;
#endif
}

void Graphics::lock() {
#ifdef USE_DD
	eraseDesc();
	if((canvas->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL))!=DD_OK)
		MessageBox(hwnd,_T("Failed to lock the surface"),_T("Surface Error"),MB_OK);
	pixels = (uint8_t *)ddsd.lpSurface;
	pitch = (int)ddsd.lPitch;
#endif
#ifdef USE_SDL
	if(SDL_MUSTLOCK(canvas)) SDL_LockSurface(canvas);
#endif
}

void Graphics::unlock() {
#ifdef USE_DD
	canvas->Unlock(0);
#endif
#ifdef USE_SDL
	if(SDL_MUSTLOCK(canvas)) SDL_UnlockSurface(canvas);
#endif
}

void Graphics::flip() {
#ifdef USE_DD
	if(app->isFullscreen()) front->Flip(back,DDFLIP_WAIT);
	else {
		POINT p;
		RECT rect;
		p.x = p.y = 0;
		ClientToScreen(hwnd,&p);
		GetClientRect(hwnd,&rect);
		OffsetRect(&rect,p.x,p.y);
		front->Blt(&rect,back,0,0,0);
	}
#endif
#ifdef USE_SDL
	SDL_Flip(screen);
#endif
}

int Graphics::getPitch() {
#ifdef USE_DD
	return pitch;
#endif
#ifdef USE_SDL
	return canvas->pitch;
#endif
}

int Graphics::getBytesPerPixel() {
#ifdef USE_DD
	return bpp;
#endif
#ifdef USE_SDL
	return canvas->format->BytesPerPixel;
#endif
}

void *Graphics::getPixels() {
#ifdef USE_DD
	return pixels;
#endif
#ifdef USE_SDL
	return canvas->pixels;
#endif
}

void Graphics::resetClip() {
#ifdef USE_DD
	setClip(0,0,width,height);
#endif
#ifdef USE_SDL
	setClip(0,0,canvas->w,canvas->h);
#endif
}

void Graphics::setClip(rect16_t &r) {
	cl = r;
#ifdef USE_DD
#endif
#ifdef USE_SDL
	SDL_SetClipRect(canvas,(SDL_Rect *)&cl);
#endif
}

void Graphics::setClip(int x,int y,int w,int h) {
	cl = (rect16_t){ x,y,w,h };
#ifdef USE_DD
#endif
#ifdef USE_SDL
	SDL_SetClipRect(canvas,(SDL_Rect *)&cl);
#endif
}

uint32_t Graphics::mapRGB(uint8_t r,uint8_t g,uint8_t b) {
#ifdef USE_DD
	return 0;
#endif
#ifdef USE_SDL
	return SDL_MapRGB(canvas->format,r,g,b);
#endif
}


void Graphics::pixel(int x,int y,uint32_t c) {
	int bpp = getBytesPerPixel();
	uint8_t *p = ((uint8_t *)getPixels())+x*bpp+y*getPitch(),*cp = (uint8_t *)&c;
	for(; bpp; --bpp) *p++ = *cp++;
}

void Graphics::draw(int x,int y,Surface s,rect16_t *src) {
#ifdef USE_DD
#endif
#ifdef USE_SDL
	rect16_t dst = { x,y,0,0 };
	SDL_BlitSurface(s,(SDL_Rect *)src,canvas,(SDL_Rect *)&dst);
#endif
}

/*#ifdef USE_DD
// Bresenhem's line algorithm
void Graphics::drawLine(int x1,int y1,int x2,int y2,uint32_t c) {
	uint8_t *p,*cp;
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
			p = pixels+x+y,cp = (uint8_t *)&c;
			for(i=0; i<bpp; i++) p[i] = cp[i];
		}
		num += numadd;
		if(num>=den) num -= den,x += xinc1,y += yinc1;
		x += xinc2;y += yinc2;
	}
}
#endif

#ifdef USE_SDL*/
// Bresenhem's line algorithm
void Graphics::drawLine(int x1,int y1,int x2,int y2,uint32_t c) {
	int pitch = getPitch(),bpp = getBytesPerPixel();
	uint8_t *pixels = (uint8_t *)getPixels(),*p,*cp;
	int xinc1,xinc2,yinc1,yinc2,den,num,numadd,npx,cpx;
	int dx = abs(x2-x1),dy = abs(y2-y1);
	int i,x = x1*bpp,y = y1*pitch;
	int l = cl.x*bpp,t = cl.y*pitch,r = (cl.x+cl.w)*bpp,b = (cl.y+cl.h)*pitch;
	if(x2>=x1) xinc1 = xinc2 = bpp;else xinc1 = xinc2 = -bpp;
	if(y2>=y1) yinc1 = yinc2 = pitch;else yinc1 = yinc2 = -pitch;
	if(dx>=dy) xinc1 = yinc2 = 0,den = dx,num = dx>>1,numadd = dy,npx = dx;
	else xinc2 = yinc1 = 0,den = dy,num = dy>>1,numadd = dx,npx = dy;
	for(cpx=0; cpx<=npx; ++cpx) {
		if(x>=l && x<r && y>=t && y<b) {
			p = pixels+x+y,cp = (uint8_t *)&c;
			for(i=0; i<bpp; ++i) p[i] = cp[i];
		}
		num += numadd;
		if(num>=den) num -= den,x += xinc1,y += yinc1;
		x += xinc2;y += yinc2;
	}
}
//#endif

void Graphics::drawDottedRect(int x,int y,int w,int h,uint32_t *p) {
	if(x+w<=cl.x || x>=cl.x+cl.w || y+h<=cl.y || y>=cl.y+cl.h) return;
	int pitch = getPitch(),bpp = getBytesPerPixel();
	uint8_t *pixels = (uint8_t *)getPixels(),*cp;
	int i,j,n,p1 = p[0],p2 = p[1];
	uint8_t *pl = pixels+x*bpp,*pt = pixels+y*pitch,*pr = pixels+(x+w-1)*bpp,*pb = pixels+(y+h-1)*pitch;
	if(x<cl.x) for(i=y,n=y+h; i<n; i++,pl+=pitch)
		for(j=0,cp=(uint8_t *)&p[2+(i+p1)%p2]; j<bpp; j++) pl[j] = cp[j];
	if(y<cl.y) for(i=x+1,n=x+w-1; i<n; i++)
		for(j=0,cp=(uint8_t *)&p[2+(i+p1)%p2]; j<bpp; j++) *pt++ = cp[j];
	if(x+w<=cl.x+cl.w) for(i=y,n=y+h; i<n; i++,pr+=pitch)
		for(j=0,cp=(uint8_t *)&p[2+(i+p1)%p2]; j<bpp; j++) pr[j] = cp[j];
	if(y+h<=cl.y+cl.h) for(i=x+1,n=x+w-1; i<n; i++)
		for(j=0,cp=(uint8_t *)&p[2+(i+p1)%p2]; j<bpp; j++) *pb++ = cp[j];
}



void Graphics::fillRect(int x,int y,int w,int h,uint32_t c) {
#ifdef USE_DD
#endif
#ifdef USE_SDL
	rect16_t r = (rect16_t){x,y,w,h};
	SDL_FillRect(canvas,(SDL_Rect *)&r,c);
#endif
}


bool Graphics::screenDump(const char *fn) {
	return Image::save(fn,canvas);
}

}; /* namespace gui */
}; /* namespace a */


