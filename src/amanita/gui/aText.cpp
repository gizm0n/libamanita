
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/gui/aText.h>


#ifdef USE_SOURCEVIEW
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#endif /* USE_SOURCEVIEW */

#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#endif /* USE_WIN32 */


#ifdef USE_SCINTILLA
scintilla_function sci_func;
#endif


Object_Inheritance(aText,aWidget)

aText::aText(widget_event_handler weh) : aWidget(weh,aWIDGET_ENTRY) {
}

aText::~aText() {
}

void aText::create(aWindow *wnd,uint32_t st) {
	char *p = text;
	text = 0;

	if((style&aTEXT_MULTILINE)) {
#ifdef USE_SCINTILLA
		if((style&aTEXT_SOURCE)==aTEXT_SOURCE) type = aWIDGET_SCINTILLA;
		else
#endif
			type = aWIDGET_TEXT;
	}
#ifdef USE_GTK
#ifdef USE_SCINTILLA
debug_output("aText::create(type: %d)\n",type);
	if(type==aWIDGET_SCINTILLA) {
		editor = scintilla_new();
		sci = SCINTILLA(editor);
		scintilla_set_id(sci,0);
		sci_func = scintilla_send_message;
		component = (aComponent)editor;
//		gtk_widget_set_usize(editor,500,300);
	} else
#endif /* USE_SCINTILLA */
	if(type==aWIDGET_TEXT) {
		component = (aComponent)gtk_scrolled_window_new(NULL,NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(component),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(component),GTK_SHADOW_IN);
#ifdef USE_SOURCEVIEW
		if((style&aTEXT_SOURCE)==aTEXT_SOURCE)
			editor = gtk_source_view_new();
		else
#else
			editor = gtk_text_view_new();
#endif /* USE_SOURCEVIEW */

		gtk_container_add(GTK_CONTAINER(component),editor);
	} else component = (aComponent)gtk_entry_new();
#endif /* USE_GTK */
#ifdef USE_WIN32
#ifdef USE_SCINTILLA
	if(type==aWIDGET_SCINTILLA && !(class_registers&aSCINTILLA_CLASS_REGISTER)) {
		class_registers |= aSCINTILLA_CLASS_REGISTER;
		LoadLibrary(_T("SciLexer.DLL"));
	} else
#endif
	if(type==aWIDGET_TEXT && !(class_registers&aTEXT_CLASS_REGISTER)) {
		class_registers |= aTEXT_CLASS_REGISTER;
		LoadLibrary(_T("RichEd20.dll"));
	}
#endif /* USE_WIN32 */
	aWidget::create(wnd,0);

#ifdef USE_SCINTILLA
	if(type==aWIDGET_SCINTILLA) {
#ifndef USE_GTK
		sci = (aScintilla)SendMessage((HWND)component,SCI_GETDIRECTPOINTER,0,0);
		sci_func = (scintilla_function)SendMessage((HWND)component,SCI_GETDIRECTFUNCTION,0,0);
#endif /* USE_GTK */
		sci_func(sci,SCI_STYLECLEARALL,0,0);
	}
#endif /* USE_SCINTILLA */

	if(p) setText(p);
}

void aText::setText(const char *str) {
	if(component) {
#ifdef USE_SCINTILLA
debug_output("aText::setText(str: %s)\n",str);
		if(type==aWIDGET_SCINTILLA)
			sci_func(sci,SCI_INSERTTEXT,0,(sptr_t)str);
		else
#endif /* USE_SCINTILLA */
#ifdef USE_GTK
		if(type==aWIDGET_TEXT) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
			gtk_text_buffer_set_text(buffer,str,-1);
		} else {
			gtk_entry_set_text(GTK_ENTRY(component),str);
		}
#endif
#ifdef USE_WIN32
		{
			tchar_t *t = tstrdup(str);
			SendMessage((HWND)component,WM_SETTEXT,0,(LPARAM)t);
			tfree(t);
		}
#endif
		if(text) {
			free(text);
			text = 0;
		}
	} else aWidget::setText(str);
}

void aText::setFont(const char *font,int sz) {
	if(component) {
#ifdef USE_SCINTILLA
		if(type==aWIDGET_SCINTILLA) {
			sci_func(sci,SCI_STYLESETFONT,STYLE_DEFAULT,(sptr_t)font);
			sci_func(sci,SCI_STYLESETSIZE,STYLE_DEFAULT,(sptr_t)sz);
		} else
#endif /* USE_SCINTILLA */
			aWidget::setFont(font,sz);
	}
}

void aText::setTabWidth(int n) {
	if(component) {
#ifdef USE_SCINTILLA
		if(type==aWIDGET_SCINTILLA) {
			sci_func(sci,SCI_SETTABWIDTH,n,0);
		} else
#endif /* USE_SCINTILLA */
		if(type==aWIDGET_TEXT) {
#ifdef USE_SOURCEVIEW
			if((style&aTEXT_SOURCE)==aTEXT_SOURCE) {
				GValue val = G_VALUE_INIT;
				g_value_init(&val,G_TYPE_INT);
				g_value_set_int(&val,3); 
				g_object_set_property(G_OBJECT(editor),"tab-width",&val);
				g_value_unset(&val);
	
			} else
#endif
			{
			}
		}
	}
}

#ifdef USE_SCINTILLA
scintilla_function aText::getScintillaFunction() {
	return sci_func;
}
#endif


