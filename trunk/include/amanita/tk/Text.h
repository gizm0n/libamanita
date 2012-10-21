#ifndef _AMANITA_TK_ENTRY_H
#define _AMANITA_TK_ENTRY_H

/**
 * @file amanita/tk/Text.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-14
 */ 


#include <amanita/Config.h>
#include <amanita/tk/Widget.h>

#ifdef USE_SCINTILLA
#include <Scintilla.h>
#include <SciLexer.h>
#ifdef USE_GTK
#include <ScintillaWidget.h>
#endif
#endif


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {



#ifdef USE_SCINTILLA
#ifdef USE_GTK
typedef ScintillaObject *aScintilla;
#else
typedef void *aScintilla;
#endif
typedef sptr_t (*scintilla_function)(aScintilla,unsigned int,uptr_t,sptr_t);
#endif


enum {
	TEXT_MULTILINE			= 0x00000001,
	TEXT_SOURCE				= 0x00000003,
};


/** Text widget.
 * 
 * @ingroup tk */
class Text : public Widget {
friend class Container;
friend class Window;
/** @cond */
Object_Instance(Text)
/** @endcond */

private:
#ifdef USE_GTK
	GtkWidget *editor;
#endif
#ifdef USE_SCINTILLA
	aScintilla sci;
#endif

public:	
	Text(widget_event_handler weh);
	virtual ~Text();

	virtual void create(Window *wnd,uint32_t st);

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

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_ENTRY_H */

