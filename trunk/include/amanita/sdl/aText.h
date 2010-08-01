#ifndef _AMANITA_SDL_TEXT_H
#define _AMANITA_SDL_TEXT_H

/**
 * @file amanita/sdl/aText.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sdl/aFont.h>


/** A class for handling multiline text.
 * 
 * aText insert linebreaks to adjust to the width and height of a frame.
 * @ingroup sdl
 */
class aText {
private:
	struct text_line {
		unsigned int pos;
		unsigned short length;
		unsigned short width;
	};
//	struct text_element {
//		unsigned int start,end,length,type,value;
//	};

	char *text;
	aFont *font;
	text_line *textLines;
	unsigned int textLen;
	unsigned int textCap;
	unsigned int textLinesLen;
	unsigned int textLinesCap;
	unsigned int maxLen;
	unsigned int width;
	unsigned int height;
	unsigned int caret;
	unsigned int caretLine;
	unsigned int select;
	unsigned int selectLine;
	int scroll;
	bool drawing;

	void resizeText(int n);
	void resizeLines(int n);
	void leftShift(int n,int len);
	void rightShift(int n,int len);
	int findLine(int pos);
	void arrange(int from);

public:
	aText(int w=0,int h=0,const char *str=0);
	~aText();

	void append(char ch) { insert(ch,textLen); }
	void append(const char *str) { insert(str,textLen); }

	void insert(char ch) { insert(ch,caret); }
	void insert(const char *str) { insert(str,caret); }
	void insert(char ch,int n);
	void insert(const char *str,int n);

	void remove(int len) { leftShift(caret,len); }
	void remove(int n,int len) { leftShift(n,len); }
	void removeAll();

	void setFont(aFont *f) { if(f && f!=font) { font = f;arrange(0); } }
	aFont *getFont() { return font; }

	void setText(const char *str);
	char *getText() { return text; }
	int length() { return textLen; }
	int lines() { return textLinesLen; }
	int lineLength(int l) { return textLines[l].length; }
	int lineWidth(int l) { return textLines[l].width; }

	void setCaret(int n) { if(textLen) caretLine = findLine(caret=n<0? 0 : (n>(int)textLen? textLen : n)); }
	void moveCaretUp() { if(caretLine>0) setCaret(caret-textLines[caretLine-1].length); }
	void moveCaretLeft() { setCaret(caret-1); }
	void moveCaretRight() { setCaret(caret+1); }
	void moveCaretDown() { setCaret(caret+textLines[caretLine].length); }
	void moveCaretUp(int n) { setCaret(textLines[n<(int)caretLine? 0 : caretLine-n].pos+getCaretCol()); }
	void moveCaretLeft(int n) { setCaret(caret-n); }
	void moveCaretRight(int n) { setCaret(caret+n); }
	void moveCaretDown(int n) { setCaret(textLines[caretLine+n>textLinesLen? textLinesLen : caretLine+n].pos+getCaretCol()); }
	void moveCaretHome() { setCaret(textLines[caretLine].pos); }
	void moveCaretEnd() { setCaret(textLines[caretLine+1].pos); }
	void moveCaretFirst() { setCaret(0); }
	void moveCaretLast() { setCaret(textLen); }
	int getCaret() { return caret; }
	int getCaretLine() { return caretLine; }
	int getCaretCol() { return caret-textLines[caretLine].pos; }

	void setScroll(int n) { scroll = n; }
	int getScroll() { return scroll; }

	void setMaxLength(int n) { maxLen = n<=0? 0 : n; }
	int getMaxLength() { return maxLen; }

	void drawText(int x,int y,int a=(ALIGN_LEFT|ALIGN_TOP),bool showCaret=0);
};

#endif /* _AMANITA_SDL_TEXT_H */
