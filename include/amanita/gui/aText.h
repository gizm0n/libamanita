#ifndef _AMANITA_GUI_ENTRY_H
#define _AMANITA_GUI_ENTRY_H

/**
 * @file amanita/gui/aText.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-14
 * @date Created: 2012-02-14
 */ 


#include <amanita/gui/aWidget.h>

#ifdef USE_SCINTILLA
#include <Scintilla.h>
#include <SciLexer.h>
#ifdef USE_GTK
#include <ScintillaWidget.h>
typedef ScintillaObject *aScintilla;
#else
typedef void *aScintilla;
#endif
typedef sptr_t (*scintilla_function)(aScintilla,unsigned int,uptr_t,sptr_t);
#endif

enum {
	aTEXT_MULTILINE			= 0x00000001,
	aTEXT_SOURCE				= 0x00000003,
};


class aText : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
aObject_Instance(aText)
/** @endcond */

private:
#ifdef USE_GTK
	GtkWidget *editor;
#endif
#ifdef USE_SCINTILLA
	aScintilla sci;
#endif

public:	
	aText(widget_event_handler weh);
	virtual ~aText();

	virtual void create(aWindow *wnd,uint32_t st);

	virtual void setText(const char *str);
	virtual void setFont(const char *font,int sz);
	void setTabWidth(int n);

#ifdef USE_GTK
	GtkWidget *getEditor() { return editor; }
#endif
#ifdef USE_SCINTILLA
	aScintilla getScintilla() { return sci; }
	scintilla_function getScintillaFunction();
#endif
};


#endif /* _AMANITA_GUI_ENTRY_H */

