
#include "../_config.h"
#include <stdio.h>
#include <amanita/gfx/aGfxGraphics.h>
#include <amanita/gfx/aGfxFont.h>
#include <amanita/gfx/aGfxTextfield.h>



Object_Inheritance(aGfxTextfield,aGfxComponent)


aGfxTextfield::_settings aGfxTextfield::_s = { 0 };

aGfxTextfield::aGfxTextfield(int x,int y,int w,int h,const char *str)
		: aGfxComponent(x,y,w,h),aGfxText(w,h,str),align(ALIGN_LEFT|ALIGN_TOP),showCaret(1),caretTimer(0) {
	aGfxText::setFont(_s.font);
	setKeyListener(this);
}

void aGfxTextfield::setDefaultSettings(aGfxFont *font) {
	_s = (_settings){ font };
}

bool aGfxTextfield::keyDown(aGfxKeyEvent &ke) {
fprintf(stderr,"aGfxTextfield::keyDown(unicode=('%c',%d),sym=('%c',%d),mod=%u)\n",
		(char)ke.unicode,ke.unicode,(char)ke.sym,ke.sym,ke.mod);
fflush(stderr);
	if(!isEnabled()) return false;
	if(ke.unicode>=' ' && ke.unicode<=0xff) insert((char)ke.unicode),moveCaretRight();
	else switch(ke.sym) {
		case SDLK_BACKSPACE:
			if(getCaret()>0) aGfxText::remove(1),moveCaretLeft();
			else return false;
			break;
		case SDLK_DELETE:
			if(getCaret()<length()) moveCaretRight(),aGfxText::remove(1),moveCaretLeft();
			else return false;
			break;
		case SDLK_PAGEUP:if(getCaret()>0) moveCaretUp(5);else return false;break;
		case SDLK_PAGEDOWN:if(getCaret()>0) moveCaretDown(5);else return false;break;
		case SDLK_UP:if(getCaret()>0) moveCaretUp();else return false;break;
		case SDLK_LEFT:if(getCaret()>0) moveCaretLeft();else return false;break;
		case SDLK_RIGHT:if(getCaret()<length()) moveCaretRight();else return false;break;
		case SDLK_DOWN:if(getCaret()>0) moveCaretDown();else return false;break;
		case SDLK_HOME:if(getCaret()>0) moveCaretHome();else return false;break;
		case SDLK_END:if(getCaret()<length()) moveCaretEnd();else return false;break;
		case SDLK_RETURN:
			if(actionListener) {
				aGfxActionEvent ae = { this,ke.mod };
				actionListener->actionPerformed(ae);
			} else { insert('\n'),moveCaretRight();break; }
			return false;
		case SDLK_ESCAPE:setKeyFocus(getParent());return false;
		default:return false;
	}
	caretTimer = 4;
	return true;
}

void aGfxTextfield::paint(time_t time) {
	drawText(getX(),getY(),align,showCaret && isEnabled()/* && hasKeyFocus()*/ && (caretTimer>0 || ((time+caretTimer)&8)));
	if(caretTimer>0 && (--caretTimer)==0) caretTimer = -(time%8);
}


