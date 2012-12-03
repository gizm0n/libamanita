
#include "../_config.h"
#include <amanita/gui/Textfield.h>
#include <amanita/gui/Textbox.h>
#include <amanita/gui/Button.h>
#include <amanita/gui/Display.h>


namespace a {
namespace gui {


Object_Inheritance(Textbox,Window)


char Textbox::yes[16];
char Textbox::no[16];
char Textbox::ok[16];
char Textbox::cancel[16];


Textbox::Textbox(int w,int h,const char *msg,unsigned int st,Font *font) : Window(0,0,w,h,CLOSE_BUTTON) {
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
	Textfield *label = new Textfield(5+fr.l->w,5+fr.t->h,w-10-fr.l->w-fr.r->w,h-10-fr.t->h-fr.b->h-b->getHeight(),msg);
	label->setTextAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	label->setFont(font);
	label->setEnabled(false);
	add(label);
}

void Textbox::showMessage(const char *msg,unsigned int st,ActionListener *al,Font *font) {
	Textbox *m = new Textbox(240,120,msg,st,font);
	m->setLocked(true);
	m->setActionListener(al);
	Display::getActiveDisplay()->add(m);
	m->show();
}

void Textbox::hideMessage(Textbox *m,int id) {
	m->hide();
	Display::getActiveDisplay()->remove(m);
	if(m->actionListener) {
		ActionEvent ae = { m,(uint32_t)id };
		m->actionListener->actionPerformed(ae);
	}
	delete m;
}

bool Textbox::keyDown(KeyEvent &ke) {
#ifdef USE_SDL
	switch(ke.sym) {
		case SDLK_ESCAPE:
			if(style==TEXTBOX_YES_NO || style==TEXTBOX_OK_CANCEL) {
				hideMessage(this,style==TEXTBOX_YES_NO? COM_ID_NO : COM_ID_CANCEL);
				break;
			}
		case SDLK_RETURN:hideMessage(this,style==TEXTBOX_YES || style==TEXTBOX_YES_NO? COM_ID_YES : COM_ID_OK);break;
		default:return false;
	}
#endif
	return true;
}

bool Textbox::actionPerformed(ActionEvent &ae) {
	hideMessage(this,ae.source->getID());
	return true;
}

}; /* namespace gui */
}; /* namespace a */


