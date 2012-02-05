
#include "../_config.h"
#include <amanita/sdl/aTextfield.h>
#include <amanita/sdl/aTextbox.h>
#include <amanita/sdl/aButton.h>
#include <amanita/sdl/aDisplay.h>



aObject_Inheritance(aTextbox,aWindow);


char aTextbox::yes[16];
char aTextbox::no[16];
char aTextbox::ok[16];
char aTextbox::cancel[16];


aTextbox::aTextbox(int w,int h,const char *msg,unsigned int st,aFont *font) : aWindow(0,0,w,h,CLOSE_BUTTON) {
	moveToCenter();
	setModal(true);
	style = st;
	aButton *b;
	int buttons = st==TEXTBOX_YES_NO || st==TEXTBOX_OK_CANCEL? 2 : 1;
	b = new aButton(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? COM_ID_YES : COM_ID_OK);
	if(buttons==2) b->setBounds(w/2-63,h-fr.b->h-25,60,20);
	else b->setBounds(w/2-60,h-fr.b->h-25,60,20);
	b->setText(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? yes : ok);
	b->setActionListener(this);
	add(b);
	if(buttons==2) {
		b = new aButton(st==TEXTBOX_YES_NO? COM_ID_NO : COM_ID_CANCEL);
		b->setBounds(w/2+3,h-fr.b->h-25,60,20);
		b->setText(st==TEXTBOX_YES_NO? no : cancel);
		b->setActionListener(this);
		add(b);
	}
	aTextfield *label = new aTextfield(5+fr.l->w,5+fr.t->h,w-10-fr.l->w-fr.r->w,h-10-fr.t->h-fr.b->h-b->getHeight(),msg);
	label->setTextAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	label->setFont(font);
	label->setEnabled(false);
	add(label);
}

void aTextbox::showMessage(const char *msg,unsigned int st,aActionListener *al,aFont *font) {
	aTextbox *m = new aTextbox(240,120,msg,st,font);
	m->setLocked(true);
	m->setActionListener(al);
	aDisplay::getActiveDisplay()->add(m);
	m->show();
}

void aTextbox::hideMessage(aTextbox *m,int id) {
	m->hide();
	aDisplay::getActiveDisplay()->remove(m);
	if(m->actionListener) {
		aActionEvent ae = { m,id };
		m->actionListener->actionPerformed(ae);
	}
	delete m;
}

bool aTextbox::keyDown(aKeyEvent &ke) {
	switch(ke.sym) {
		case SDLK_ESCAPE:
			if(style==TEXTBOX_YES_NO || style==TEXTBOX_OK_CANCEL) {
				hideMessage(this,style==TEXTBOX_YES_NO? COM_ID_NO : COM_ID_CANCEL);
				break;
			}
		case SDLK_RETURN:hideMessage(this,style==TEXTBOX_YES || style==TEXTBOX_YES_NO? COM_ID_YES : COM_ID_OK);break;
		default:return false;
	}
	return true;
}

bool aTextbox::actionPerformed(aActionEvent &ae) {
	hideMessage(this,ae.source->getID());
	return true;
}


