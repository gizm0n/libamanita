
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <libamanita/sdl/Graphics.h>
#include <libamanita/sdl/Font.h>


RttiObjectInheritance(Font,Object);


Font::Font(const char *file,int size,int style,long color,int render)
		: Object(),adjx(0),adjy(0),spacing(0) {
	SDL_Color fg = { (char)(color>>16),(char)(color>>8),(char)(color),255 };
	SDL_Color bg = { 255,255,255,255 };

// ::fprintf(stderr,"Font::Font(1)\n");
// fflush(stderr);
	TTF_Font *font = TTF_OpenFont(file,size);
	if(!font) {
::fprintf(stderr,"can't open font: %s\n",file);
fflush(stderr);
	}
	name = strdup(file);
//::fprintf(stderr,"Font::Font(2.1)\n");
//fflush(stderr);
	if(style!=TTF_GetFontStyle(font)) TTF_SetFontStyle(font,style);
//::fprintf(stderr,"Font::Font(2.2)\n");
//fflush(stderr);
	height = TTF_FontHeight(font);
	ascent = TTF_FontAscent(font);
	descent = TTF_FontDescent(font);
	lineskip = TTF_FontLineSkip(font);
::fprintf(stderr,"Font::Font(%s,size=%d,style=%d,height=%d,ascent=%d,descent=%d,lineskip=%d)\n",
		file,size,style,height,ascent,descent,lineskip);
fflush(stderr);
	for(int i=0; i<256; i++) {
		switch(render) {
			case 0:glyphs[i].surface = TTF_RenderGlyph_Solid(font,i,fg);break;
			case 1:glyphs[i].surface = TTF_RenderGlyph_Shaded(font,i,fg,bg);break;
			case 2:glyphs[i].surface = TTF_RenderGlyph_Blended(font,i,fg);break;
		}
		TTF_GlyphMetrics(font,i,&glyphs[i].minx,&glyphs[i].maxx,&glyphs[i].miny,&glyphs[i].maxy,&glyphs[i].advance);

//::fprintf(stderr,"Glyph['%c',%d] minx=%d,maxx=%d,miny=%d,maxy=%d,advance=%d\n",(char)i,i,glyphs[i].minx,glyphs[i].maxx,glyphs[i].miny,glyphs[i].maxy,glyphs[i].advance);
//fflush(stderr);
	}
//::fprintf(stderr,"Font::Font(3)\n");
//fflush(stderr);
	TTF_CloseFont(font);
//::fprintf(stderr,"Font::Font(4)\n");
//fflush(stderr);
}


Font::~Font() {
::fprintf(stderr,"Font::~Font(name=%s)\n",name);
fflush(stderr);
	if(name) { free(name);name = 0; }
	for(int i=0; i<256; i++) {
// ::fprintf(stderr,"Font::~Font(i=%d,c=%c)\n",i,(char)i);
// fflush(stderr);
		if(glyphs[i].surface) SDL_FreeSurface(glyphs[i].surface);
		glyphs[i].surface = 0;
	}
// ::fprintf(stderr,"Font::~Font(done)\n");
// fflush(stderr);
}

int Font::stringWidth(const char *str) {
	int w = 0;
	for(unsigned char c; (c=(unsigned char)*str)!='\0'; str++,w+=spacing) w += glyphs[c].advance;
	return w;
}

int Font::stringWidth(const char *str,int i,int l) {
	int w = 0;
	while(*str && i>0) str++,i--;
	if(i>0) return 0;
	for(unsigned char c; (i<l || l<0) && (c=(unsigned char)*str)!='\0'; i++,str++,w+=spacing) w += glyphs[c].advance;
	return w;
}

void Font::putchar(int x,int y,unsigned char c) {
	glyph &gl = glyphs[c];
	if(gl.surface) {
		SDL_Rect r = { adjx+x+gl.minx,adjy+y-gl.maxy };
		SDL_BlitSurface(gl.surface,0,g.getCanvas(),&r);
	}
}

void Font::print(int x,int y,const char *str) {
	SDL_Rect r;
	unsigned char c;
	x += adjx,y += adjy;
	while((c=*str++)!='\0' && c!='\n') {
		glyph &gl = glyphs[c];
		r.x = x+gl.minx,r.y = y-gl.maxy;
		if(gl.surface) SDL_BlitSurface(gl.surface,0,g.getCanvas(),&r);
		x += gl.advance+spacing;
	}
}

void Font::print(int x,int y,int a,const char *str) {
	if(a&ALIGN_CENTER) x -= stringWidth(str)/2;
	else if(a&ALIGN_RIGHT) x -= stringWidth(str);
	if(a&ALIGN_MIDDLE) y += height/2;
	else if(a&ALIGN_BOTTOM) y += height;
	print(x,y,str);  
}

void Font::printf(int x,int y,const char *format, ...) {
	char text[1024];
	va_list args;
   va_start(args,format);
	vsprintf(text,format,args);
   va_end(args);
	print(x,y,text);
}

void Font::printf(int x,int y,int a,const char *format, ...) {
	char text[1024];
	va_list args;
   va_start(args,format);
	vsprintf(text,format,args);
   va_end(args);
	print(x,y,a,text);
}

