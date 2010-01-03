
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>


aObject_Inheritance(aFont,aObject);


aFont::aFont(const char *file,int size,int style,long color,int render) : aObject() {
	SDL_Color fg = { (char)(color>>16),(char)(color>>8),(char)(color),255 };
	SDL_Color bg = { 255,255,255,255 };

	metrics.adjx = 0;
	metrics.adjy = 0;
	metrics.spacing = 0;

// ::fprintf(stderr,"aFont::aFont(1)\n");
// fflush(stderr);
	TTF_Font *font = TTF_OpenFont(file,size);
	if(!font) {
		::fprintf(stderr,"can't open font: %s\n",file);
		fflush(stderr);
	}
	name = strdup(file);
//::fprintf(stderr,"aFont::aFont(2.1)\n");
//fflush(stderr);
	if(style!=TTF_GetFontStyle(font)) TTF_SetFontStyle(font,style);
//::fprintf(stderr,"aFont::aFont(2.2)\n");
//fflush(stderr);
	metrics.height = TTF_FontHeight(font);
	metrics.ascent = TTF_FontAscent(font);
	metrics.descent = TTF_FontDescent(font);
	metrics.lineskip = TTF_FontLineSkip(font);
::fprintf(stderr,"aFont::aFont(%s,size=%d,style=%d,height=%d,ascent=%d,descent=%d,lineskip=%d)\n",
		file,size,style,metrics.height,metrics.ascent,metrics.descent,metrics.lineskip);
fflush(stderr);
	for(int i=0; i<256; i++) {
		switch(render) {
			case 0:glyphs[i] = TTF_RenderGlyph_Solid(font,i,fg);break;
			case 1:glyphs[i] = TTF_RenderGlyph_Shaded(font,i,fg,bg);break;
			case 2:glyphs[i] = TTF_RenderGlyph_Blended(font,i,fg);break;
		}
		TTF_GlyphMetrics(font,i,
			&metrics.glyphs[i].minx,
			&metrics.glyphs[i].maxx,
			&metrics.glyphs[i].miny,
			&metrics.glyphs[i].maxy,
			&metrics.glyphs[i].advance);

//::fprintf(stderr,"Glyph['%c',%d] minx=%d,maxx=%d,miny=%d,maxy=%d,advance=%d\n",(char)i,i,glyphs[i].minx,glyphs[i].maxx,glyphs[i].miny,glyphs[i].maxy,glyphs[i].advance);
//fflush(stderr);
	}
//::fprintf(stderr,"aFont::aFont(3)\n");
//fflush(stderr);
	TTF_CloseFont(font);
//::fprintf(stderr,"aFont::aFont(4)\n");
//fflush(stderr);
}


aFont::~aFont() {
::fprintf(stderr,"aFont::~aFont(name=%s)\n",name);
fflush(stderr);
	if(name) { free(name);name = 0; }
	for(int i=0; i<256; i++) {
// ::fprintf(stderr,"aFont::~aFont(i=%d,c=%c)\n",i,(char)i);
// fflush(stderr);
		if(glyphs[i]) SDL_FreeSurface(glyphs[i]);
		glyphs[i] = 0;
	}
// ::fprintf(stderr,"aFont::~aFont(done)\n");
// fflush(stderr);
}

int aFont::stringWidth(const char *str) {
	int w = 0;
	for(unsigned char c; (c=(unsigned char)*str)!='\0'; str++,w+=metrics.spacing)
		w += metrics.glyphs[c].advance;
	return w;
}

int aFont::stringWidth(const char *str,int i,int l) {
	int w = 0;
	while(*str && i>0) str++,i--;
	if(i>0) return 0;
	for(unsigned char c; (i<l || l<0) && (c=(unsigned char)*str)!='\0'; i++,str++,w+=metrics.spacing)
		w += metrics.glyphs[c].advance;
	return w;
}

void aFont::putchar(int x,int y,unsigned char c) {
	if(glyphs[c]) {
		aGlyphmetrics &gl = metrics.glyphs[c];
		SDL_Rect r = { metrics.adjx+x+gl.minx,metrics.adjy+y-gl.maxy };
		SDL_BlitSurface(glyphs[c],0,g.getCanvas(),&r);
	}
}

void aFont::print(int x,int y,const char *str) {
	SDL_Rect r;
	unsigned char c;
	x += metrics.adjx,y += metrics.adjy;
	while((c=*str++)!='\0' && c!='\n') {
		aGlyphmetrics &gl = metrics.glyphs[c];
		r.x = x+gl.minx,r.y = y-gl.maxy;
		if(glyphs[c]) SDL_BlitSurface(glyphs[c],0,g.getCanvas(),&r);
		x += gl.advance+metrics.spacing;
	}
}

void aFont::print(int x,int y,int a,const char *str) {
	if(a&ALIGN_CENTER) x -= stringWidth(str)/2;
	else if(a&ALIGN_RIGHT) x -= stringWidth(str);
	if(a&ALIGN_MIDDLE) y += metrics.height/2;
	else if(a&ALIGN_BOTTOM) y += metrics.height;
	print(x,y,str);  
}

void aFont::printf(int x,int y,const char *format, ...) {
	char text[1024];
	va_list args;
   va_start(args,format);
	vsprintf(text,format,args);
   va_end(args);
	print(x,y,text);
}

void aFont::printf(int x,int y,int a,const char *format, ...) {
	char text[1024];
	va_list args;
   va_start(args,format);
	vsprintf(text,format,args);
   va_end(args);
	print(x,y,a,text);
}

