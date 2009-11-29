
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libamanita/sdl/Graphics.h>
#include <libamanita/sdl/Text.h>



Text::Text(int w,int h,const char *str) {
	text = 0,textLines = 0,width = w,height = h,maxLen = 0,drawing = false;
	font = 0;
	setText(str);
}

Text::~Text() {
	if(text) { free(text);text = 0,textLen = 0; }
	if(textLines) { delete[] textLines;textLines = 0,textLinesLen = 0; }
}

void Text::resizeText(int n) {
	while(drawing);
	if(n<0xff) n = 0xff;
	if((int)textCap<n) textCap = n;
	if(textCap<1024) textCap <<= 1;
	else textCap += 1024;
	char *t = text;
	text = (char *)malloc(textCap);
	memset(&text[textLen],0,textCap-textLen);
	if(t) {
		memcpy(text,t,textLen);
		free(t);
	}
}

void Text::resizeLines(int n) {
	while(drawing);
	if(n<0xff) n = 0xff;
	if((int)textLinesCap<n) textLinesCap = n;
	if(textLinesCap<1024) textLinesCap <<= 1;
	else textLinesCap += 1024;
	TextLine *t = new TextLine[textLinesCap];
	memset(&t[textLinesLen],0,(textLinesCap-textLinesLen)*sizeof(TextLine));
	if(textLines) {
		for(unsigned int i=0; i<textLinesLen; i++) t[i] = textLines[i];
		delete[] textLines;
	}
	textLines = t;
}

void Text::leftShift(int n,int len) {
	if(!len || !textLen) return;
	if(n<len) len = n;
	else if(n>(int)textLen) n = textLen;
	for(int i=n; i<=(int)textLen; i++) text[i-len] = text[i];
	if(n<(int)textLen) arrange(n);
	textLen -= len;
}

void Text::rightShift(int n,int len) {
	if(!len) return;
	if(textLen+len>=textCap) resizeText(len);
	if(n>(int)textLen) n = textLen;
	for(int i=textLen; i>=n; i--) text[i+len] = text[i];
	textLen += len;
}

void Text::insert(char ch,int n) {
fprintf(stderr,"Text::insert('%c',%d)\n",ch,n);
fflush(stderr);
	if(maxLen && textLen>=maxLen) return;
	if(n>=(int)textLen) n = textLen;
	rightShift(n,1);
	text[n] = ch;
	arrange(n);
}

void Text::insert(const char *str,int n) {
	if(!str || !*str) return;
	if(n>=(int)textLen) n = textLen;
	int l = strlen(str),i;
	if(maxLen && textLen+l>maxLen) return;
	rightShift(n,l);
	if(n==(int)textLen) l++;
	for(i=n; *str; i++,str++) text[i] = *str;
	arrange(n);
}

void Text::removeAll() {
	while(drawing);
	if(textLines) delete[] textLines;
	if(text) free(text);
	text = 0,textLen = 0,textCap = 0,textLines = 0,textLinesLen = 0,textLinesCap = 0;
	caret = 0,caretLine = 0,select = 0,selectLine = 0,scroll = -1;
}

void Text::setText(const char *str) {
	while(drawing);
	char *t = text;
	if(textLines) delete[] textLines;
	text = 0,textLen = 0,textCap = 0,textLines = 0,textLinesLen = 0,textLinesCap = 0;
	caret = 0,caretLine = 0,select = 0,selectLine = 0,scroll = -1;
	insert(str,0);
	if(t) free(t);
}

int Text::findLine(int pos) {
	for(int i=textLinesLen-1; i>=0; i--) if(pos>=(int)textLines[i].pos) return i;
	return 0;
}

void Text::arrange(int from) {
//fprintf(stderr,"Text::arrange(from=%d,textLen=%d,width=%d)\n",from,textLen,width);
//fflush(stderr);
	if(!font || !width || !textLen) return;
	short w = 0,len,i;
	int l = 0,w1,w2,p = 0;
	if(textLinesCap) p = textLines[l=(from==(int)caret? caretLine : findLine(from))].pos;
	unsigned char ch;
	while(true) {
		for(len=0,w=0,w1=0,w2=0,i=0,ch=0; true; i++) {
			if(ch==' ') len = i,w2 = w;
			ch = text[p++];
			if(ch>=' ') w1 = w,w += font->glyphs[ch].advance;
			if(p>(int)textLen || w>(int)width || ch=='\n') {
				if(p>(int)textLen) len = i;
				else if(ch=='\n') len = ++i;
				else if(len==0) len = i++,w = w1;
				else if(w>(int)width) w = w2;
				else len = i;
				break;
			}
		}
		if(l+1>=(int)textLinesCap) resizeLines(0);
		textLines[l].length = len,textLines[l].width = w;
//fprintf(stderr,"Text::arrange(line=%d,length=%d,width=%d,pos=%d)\n",l,textLines[l].length,textLines[l].width,p);
//fflush(stderr);
		p -= i-len;
		textLines[++l].pos = p;
		if(p>=(int)textLen) break;
	}
	textLinesLen = l;
//fprintf(stderr,"Text::arrange(textLines=%d)\n",textLinesLen);
//fflush(stderr);
}

void Text::drawText(int x,int y,int a,bool showCaret) {
	if(!font) return;
	drawing = true;
	int i = 0;
	SDL_Rect r;
	r.x = x,r.y = y;
	SDL_Surface *canvas = g.getCanvas();
	if(textLinesLen) {
		x += font->adjx,y += font->adjy;
		int sc = scroll,w,h = font->getLineSkip(),l = 0,j = 0,n = 0;
		if(sc<0) {
			if(a&ALIGN_MIDDLE) sc = ((textLinesLen*h)>>1)-(height>>1);
			else if(a&ALIGN_BOTTOM) sc = (textLinesLen*h)-height;
			else sc = 0;
		}
		if(sc<0) i = -sc,sc = 0;
		else if(sc>0) i = h-(sc%h),l = (sc/h)+(i>=(h>>1)),n = textLines[l].pos;
		unsigned char ch = 0;
		for(; l<(int)textLinesLen && i+h<=(int)height; l++,i+=h) {
			r.x = x,r.y = y+i;
			TextLine &tl = textLines[l];
			if(a&ALIGN_CENTER) r.x += (width-tl.width)>>1;
			else if(a&ALIGN_RIGHT) r.x += (width-tl.width);
			for(j=0,n=tl.pos; j<tl.length; j++,n++) {
				if(showCaret && (int)caret==n) {
					r.y = y+i-font->glyphs[(int)'|'].maxy;
					SDL_BlitSurface(font->glyphs[(int)'|'].surface,0,canvas,&r);
				}
				if((ch=text[n])>=' ') {
					r.x += font->glyphs[ch].minx,w = font->glyphs[ch].advance-font->glyphs[ch].minx;
					r.y = y+i-font->glyphs[ch].maxy;
					SDL_BlitSurface(font->glyphs[ch].surface,0,canvas,&r);
					r.x += w;
				}
			}
		}
		if(ch=='\n') r.x = x;
		else i -= h;
	}
	if(showCaret && caret==textLen) {
		r.y = y+i-font->glyphs[(int)'|'].maxy;
		SDL_BlitSurface(font->glyphs[(int)'|'].surface,0,canvas,&r);
	}
	drawing = false;
}

