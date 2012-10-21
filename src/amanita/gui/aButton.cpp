
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#include <commctrl.h>
#endif
#include <amanita/gui/aButton.h>
#include <amanita/gui/aWindow.h>


#ifdef USE_GTK
void button_clicked_callback(GtkWidget *widget,gpointer data) {
	aButton *b = (aButton *)data;
	widget_event_handler weh = b->getEventHandler();
	weh(b,aBUTTON_CLICKED,0,0,0);
}

void button_toggled_callback(GtkWidget *widget,gpointer data) {
	aButton *b = (aButton *)data;
	widget_event_handler weh = b->getEventHandler();
	bool a = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b->getComponent()));
	if(!b->isRadioButton() || a)
		weh(b,aBUTTON_TOGGLED,(intptr_t)a,0,0);
}
#endif

Object_Inheritance(aButton,aWidget)

aButton::aButton(widget_event_handler weh) : aWidget(weh,aWIDGET_BUTTON),group(0) {
}

aButton::~aButton() {
}

void aButton::create(aWindow *wnd,uint32_t st) {
	if((style&aBUTTON_CHECKBOX)) type = aWIDGET_CHECKBOX;
	else if((style&aBUTTON_RADIOBUTTON)) type = aWIDGET_RADIOBUTTON;
	st = 0;
#ifdef USE_GTK
	if(type==aWIDGET_BUTTON) {
		component = (aComponent)gtk_button_new_with_label(text? text : "");
		if(event_handler) g_signal_connect(G_OBJECT(component),"clicked",G_CALLBACK(button_clicked_callback),this);
	} else {
		if(type==aWIDGET_CHECKBOX) component = (aComponent)gtk_check_button_new_with_label(text? text : "");
		else if(type==aWIDGET_RADIOBUTTON) {
			if(group) component = (aComponent)gtk_radio_button_new_with_label_from_widget((GtkRadioButton *)group->component,text? text : "");
			else component = (aComponent)gtk_radio_button_new_with_label(0,text? text : "");
		}
		if(event_handler) g_signal_connect(G_OBJECT(component),"toggled",G_CALLBACK(button_toggled_callback),this);
	}
	if((style&aBUTTON_ALIGNED))
		gtk_misc_set_alignment(GTK_MISC(component),
			(style&aBUTTON_LEFT)? ((style&aBUTTON_CENTER)==aBUTTON_CENTER? 0.5 : 0) : ((style&aBUTTON_RIGHT)? 1.0 : 0),
			(style&aBUTTON_TOP)? ((style&aBUTTON_MIDDLE)==aBUTTON_MIDDLE? 0.5 : 0) : ((style&aBUTTON_BOTTOM)? 1.0 : 0.5));
#endif
#ifdef USE_WIN32
	if(type==aWIDGET_BUTTON) {
		if((style&aBUTTON_DEFAULT)) st = BS_DEFPUSHBUTTON;
		else st = BS_PUSHBUTTON;
	}
	if((style&aBUTTON_ALIGNED)) {
		if((style&aBUTTON_LEFT)) {
			if((style&aBUTTON_CENTER)==aBUTTON_CENTER) st |= BS_CENTER;
			else st |= BS_LEFT;
		} else if((style&aBUTTON_RIGHT)) st |= BS_RIGHT;
		if((style&aBUTTON_TOP)) {
			if((style&aBUTTON_MIDDLE)==aBUTTON_MIDDLE) st |= BS_VCENTER;
			else st |= BS_TOP;
		} else if((style&aBUTTON_BOTTOM)) st |= BS_BOTTOM;
	}
#endif

	aWidget::create(wnd,st);
	if((style&aBUTTON_CHECKED)) setChecked(true);
}

void aButton::setText(const char *str) {
	aWidget::setText(str);
	if(component) {
	// If component, change text.
	}
}

void aButton::setChecked(bool s) {
	if(type==aWIDGET_BUTTON) return;
	if(component) {
#ifdef USE_GTK
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(component),s);
#endif
#ifdef USE_WIN32
		SendMessage((HWND)component,BM_SETCHECK,(WPARAM)(s? BST_CHECKED : BST_UNCHECKED),0);
#endif
	} else if(s) style |= aBUTTON_CHECKED;
	else style &= ~aBUTTON_CHECKED;
}

bool aButton::isChecked() {
	if(type==aWIDGET_BUTTON) return false;
	if(component) {
#ifdef USE_GTK
		return (bool)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(component));
#endif
#ifdef USE_WIN32
		return SendMessage((HWND)component,BM_GETCHECK,0,0)==BST_CHECKED;
#endif
	} else return (style&aBUTTON_CHECKED)? true : false;
}


