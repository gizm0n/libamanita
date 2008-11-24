
#include "../config.h"
#include <libamanita/gui/TextField.h>
#include <libamanita/gui/TextBox.h>
#include <libamanita/gui/Button.h>
#include <libamanita/gui/Display.h>



RttiObjectInheritance(TextBox,Window);


char TextBox::yes[16];
char TextBox::no[16];
char TextBox::ok[16];
char TextBox::cancel[16];


TextBox::TextBox(int w,int h,char *msg,unsigned int st,Font *font) : Window(0,0,w,h,CLOSE_BUTTON) {
	moveToCenter();
	setModal(true);
	style = st;
	Button *b;
	int buttons = st==TEXTBOX_YES_NO || st==TEXTBOX_OK_CANCEL? 2 : 1;
	b = new Button(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? COM_ID_YES : COM_ID_OK);
	if(buttons==2) b->setBounds(w/2-63,h-fr.b->h-25,60,20);
	else b->setBounds(w/2-60,h-fr.b->h-25,60,20);
	b->setText(st==TEXTBOX_YES || st==TEXTBOX_YES_NO? yes : ok);
	b->setActionListener(this);
	add(b);
	if(buttons==2) {
		b = new Button(st==TEXTBOX_YES_NO? COM_ID_NO : COM_ID_CANCEL);
		b->setBounds(w/2+3,h-fr.b->h-25,60,20);
		b->setText(st==TEXTBOX_YES_NO? no : cancel);
		b->setActionListener(this);
		add(b);
	}
	TextField *label = new TextField(5+fr.l->w,5+fr.t->h,w-10-fr.l->w-fr.r->w,h-10-fr.t->h-fr.b->h-b->getHeight(),msg);
	label->setTextAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	label->setFont(font);
	label->setEnabled(false);
	add(label);
}

void TextBox::showMessage(char *msg,unsigned int st,ActionListener *al,Font *font) {
	TextBox *m = new TextBox(240,120,msg,st,font);
	m->setLocked(true);
	m->setActionListener(al);
	Display::getActiveDisplay()->add(m);
	m->show();
}

void TextBox::hideMessage(TextBox *m,int id) {
	m->hide();
	Display::getActiveDisplay()->remove(m);
	if(m->actionListener) {
		ActionEvent ae = { m,id };
		m->actionListener->actionPerformed(ae);
	}
	delete m;
}

bool TextBox::keyDown(KeyEvent &ke) {
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

bool TextBox::actionPerformed(ActionEvent &ae) {
	hideMessage(this,ae.source->getID());
	return true;
}


