
#include "../config.h"
#include <stdio.h>
#include <libamanita/sdl/Graphics.h>
#include <libamanita/sdl/Font.h>
#include <libamanita/gui/TextField.h>



RttiObjectInheritance(TextField,Component);


TextField::_settings TextField::_s = { 0 };

TextField::TextField(int x,int y,int w,int h,char *str)
		: Component(x,y,w,h),Text(w,h,str),align(ALIGN_LEFT|ALIGN_TOP),showCaret(1),caretTimer(0) {
	Text::setFont(_s.font);
	setKeyListener(this);
}

void TextField::setDefaultSettings(Font *font) {
	_s = (_settings){ font };
}

bool TextField::keyDown(KeyEvent &ke) {
fprintf(stderr,"TextField::keyDown(unicode=('%c',%d),sym=('%c',%d),mod=%u)\n",
		(char)ke.unicode,ke.unicode,(char)ke.sym,ke.sym,ke.mod);
fflush(stderr);
	if(!isEnabled()) return false;
	if(ke.unicode>=' ' && ke.unicode<=0xff) insert((char)ke.unicode),moveCaretRight();
	else switch(ke.sym) {
		case SDLK_BACKSPACE:
			if(getCaret()>0) Text::remove(1),moveCaretLeft();
			else return false;
			break;
		case SDLK_DELETE:
			if(getCaret()<length()) moveCaretRight(),Text::remove(1),moveCaretLeft();
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
				ActionEvent ae = { this,ke.mod };
				actionListener->actionPerformed(ae);
			} else { insert('\n'),moveCaretRight();break; }
			return false;
		case SDLK_ESCAPE:setKeyFocus(getParent());return false;
		default:return false;
	}
	caretTimer = 4;
	return true;
}

void TextField::paint(int time) {
	drawText(getX(),getY(),align,showCaret && isEnabled()/* && hasKeyFocus()*/ && (caretTimer>0 || ((time+caretTimer)&8)));
	if(caretTimer>0 && (--caretTimer)==0) caretTimer = -(time%8);
}


