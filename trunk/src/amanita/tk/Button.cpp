
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#include <commctrl.h>
#endif
#include <amanita/tk/Button.h>
#include <amanita/tk/Window.h>


namespace a {
namespace tk {



#ifdef USE_GTK
void button_clicked_callback(GtkWidget *widget,gpointer data) {
	Button *b = (Button *)data;
	widget_event_handler weh = b->getEventHandler();
	weh(b,BUTTON_CLICKED,0,0,0);
}

void button_toggled_callback(GtkWidget *widget,gpointer data) {
	Button *b = (Button *)data;
	widget_event_handler weh = b->getEventHandler();
	bool a = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b->getComponent()));
	if(!b->isRadioButton() || a)
		weh(b,BUTTON_TOGGLED,(intptr_t)a,0,0);
}
#endif

Object_Inheritance(Button,Widget)

Button::Button(widget_event_handler weh) : Widget(weh,WIDGET_BUTTON),group(0) {
}

Button::~Button() {
}

void Button::create(Window *wnd,uint32_t st) {
	if((style&BUTTON_CHECKBOX)) type = WIDGET_CHECKBOX;
	else if((style&BUTTON_RADIOBUTTON)) type = WIDGET_RADIOBUTTON;
	st = 0;
#ifdef USE_GTK
	if(type==WIDGET_BUTTON) {
		component = (Component)gtk_button_new_with_label(text? text : "");
		if(event_handler) g_signal_connect(G_OBJECT(component),"clicked",G_CALLBACK(button_clicked_callback),this);
	} else {
		if(type==WIDGET_CHECKBOX) component = (Component)gtk_check_button_new_with_label(text? text : "");
		else if(type==WIDGET_RADIOBUTTON) {
			if(group) component = (Component)gtk_radio_button_new_with_label_from_widget((GtkRadioButton *)group->component,text? text : "");
			else component = (Component)gtk_radio_button_new_with_label(0,text? text : "");
		}
		if(event_handler) g_signal_connect(G_OBJECT(component),"toggled",G_CALLBACK(button_toggled_callback),this);
	}
	if((style&BUTTON_ALIGNED))
		gtk_misc_set_alignment(GTK_MISC(component),
			(style&BUTTON_LEFT)? ((style&BUTTON_CENTER)==BUTTON_CENTER? 0.5 : 0) : ((style&BUTTON_RIGHT)? 1.0 : 0),
			(style&BUTTON_TOP)? ((style&BUTTON_MIDDLE)==BUTTON_MIDDLE? 0.5 : 0) : ((style&BUTTON_BOTTOM)? 1.0 : 0.5));
#endif
#ifdef USE_WIN32
	if(type==WIDGET_BUTTON) {
		if((style&BUTTON_DEFAULT)) st = BS_DEFPUSHBUTTON;
		else st = BS_PUSHBUTTON;
	}
	if((style&BUTTON_ALIGNED)) {
		if((style&BUTTON_LEFT)) {
			if((style&BUTTON_CENTER)==BUTTON_CENTER) st |= BS_CENTER;
			else st |= BS_LEFT;
		} else if((style&BUTTON_RIGHT)) st |= BS_RIGHT;
		if((style&BUTTON_TOP)) {
			if((style&BUTTON_MIDDLE)==BUTTON_MIDDLE) st |= BS_VCENTER;
			else st |= BS_TOP;
		} else if((style&BUTTON_BOTTOM)) st |= BS_BOTTOM;
	}
#endif

	Widget::create(wnd,st);
	if((style&BUTTON_CHECKED)) setChecked(true);
}

void Button::setText(const char *str) {
	Widget::setText(str);
	if(component) {
	// If component, change text.
	}
}

void Button::setChecked(bool s) {
	if(type==WIDGET_BUTTON) return;
	if(component) {
#ifdef USE_GTK
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(component),s);
#endif
#ifdef USE_WIN32
		SendMessage((HWND)component,BM_SETCHECK,(WPARAM)(s? BST_CHECKED : BST_UNCHECKED),0);
#endif
	} else if(s) style |= BUTTON_CHECKED;
	else style &= ~BUTTON_CHECKED;
}

bool Button::isChecked() {
	if(type==WIDGET_BUTTON) return false;
	if(component) {
#ifdef USE_GTK
		return (bool)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(component));
#endif
#ifdef USE_WIN32
		return SendMessage((HWND)component,BM_GETCHECK,0,0)==BST_CHECKED;
#endif
	} else return (style&BUTTON_CHECKED)? true : false;
}

}; /* namespace tk */
}; /* namespace a */


