
#include "../_config.h"
#include <amanita/gfx/aGfxTextfield.h>
#include <amanita/gfx/aGfxTextbox.h>
#include <amanita/gfx/aGfxButton.h>
#include <amanita/gfx/aGfxDisplay.h>



Object_Inheritance(aGfxTextbox,aGfxWindow)


char aGfxTextbox::yes[16];
char aGfxTextbox::no[16];
char aGfxTextbox::ok[16];
char aGfxTextbox::cancel[16];


aGfxTextbox::aGfxTextbox(int w,int h,const char *msg,unsigned int st,aGfxFont *font) : aGfxWindow(0,0,w,h,CLOSE_BUTTON) {
	moveToCenter();
	setModal(true);
	style = st;
	aGfxButton *b;
	int buttons = st==TEXTBOX_YES_NO || st==TEXTBOX_OK_CANCEL? 2 : 1;
	b = new aGfxButton(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? COM_ID_YES : COM_ID_OK);
	if(buttons==2) b->setBounds(w/2-63,h-fr.b->h-25,60,20);
	else b->setBounds(w/2-60,h-fr.b->h-25,60,20);
	b->setText(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? yes : ok);
	b->setActionListener(this);
	add(b);
	if(buttons==2) {
		b = new aGfxButton(st==TEXTBOX_YES_NO? COM_ID_NO : COM_ID_CANCEL);
		b->setBounds(w/2+3,h-fr.b->h-25,60,20);
		b->setText(st==TEXTBOX_YES_NO? no : cancel);
		b->setActionListener(this);
		add(b);
	}
	aGfxTextfield *label = new aGfxTextfield(5+fr.l->w,5+fr.t->h,w-10-fr.l->w-fr.r->w,h-10-fr.t->h-fr.b->h-b->getHeight(),msg);
	label->setTextAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	label->setFont(font);
	label->setEnabled(false);
	add(label);
}

void aGfxTextbox::showMessage(const char *msg,unsigned int st,aGfxActionListener *al,aGfxFont *font) {
	aGfxTextbox *m = new aGfxTextbox(240,120,msg,st,font);
	m->setLocked(true);
	m->setActionListener(al);
	aGfxDisplay::getActiveDisplay()->add(m);
	m->show();
}

void aGfxTextbox::hideMessage(aGfxTextbox *m,int id) {
	m->hide();
	aGfxDisplay::getActiveDisplay()->remove(m);
	if(m->actionListener) {
		aGfxActionEvent ae = { m,id };
		m->actionListener->actionPerformed(ae);
	}
	delete m;
}

bool aGfxTextbox::keyDown(aGfxKeyEvent &ke) {
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

bool aGfxTextbox::actionPerformed(aGfxActionEvent &ae) {
	hideMessage(this,ae.source->getID());
	return true;
}


