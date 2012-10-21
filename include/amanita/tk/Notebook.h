#ifndef _AMANITA_TK_NOTEBOOK_H
#define _AMANITA_TK_NOTEBOOK_H

/**
 * @file amanita/tk/Notebook.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-07
 */ 

#include <amanita/Vector.h>
#include <amanita/tk/Widget.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	NOTEBOOK_HIDE_TABS		= 0x00000001,
	NOTEBOOK_CLOSE_BUTTON	= 0x00000002,
};

enum {
	NOTEBOOK_PAGE_SHOW		= 0x00000001,
	NOTEBOOK_PAGE_HIDE		= 0x00000002,
};


/** @cond */
class Notebook;
/** @endcond */


/** Notebook tab.
 * @ingroup tk */
struct NotebookTab {
	Notebook *notebook;
/** @cond */
#ifdef USE_GTK
	GtkWidget *button;
	GtkWidget *label;
#endif
/** @endcond */
	uint8_t index;
	char *name;
};


/** Notebook page.
 * @ingroup tk */
class NotebookPage : public Object {
friend class Notebook;
/** @cond */
Object_Instance(NotebookPage)
/** @endcond */

protected:
	NotebookTab tab;
	Widget *page;
	int status;

public:
	NotebookPage(Notebook *nb);
	virtual ~NotebookPage();

	void open();
	void close();

	void setTabLabel(const char *str);
};


/** Notebook.
 * 
 * @ingroup tk */
class Notebook : public Widget {
friend class Window;
/** @cond */
Object_Instance(Notebook)
/** @endcond */

private:
	Vector pages;
	int selected;

#ifdef USE_WIN32
/** @cond */
	RECT client;

	void tabEvent();
/** @endcond */
	virtual void makeLayout(int x,int y,int w,int h);
	virtual void move();
#endif

public:
	Notebook(widget_event_handler weh);
	virtual ~Notebook();

	virtual void create(Window *wnd,uint32_t st);

	int openPage(const char *tab,Widget *page,bool sel=true);
	int openPage(NotebookPage *np,bool sel=true);
	void selectPage(int n);
	void selectPage(NotebookPage *np) { selectPage(np->tab.index); }
	const char *getTab(int n=-1);
	NotebookPage *getPage(int n=-1);
	void closePage(int n=-1);
	void closePage(NotebookPage *np) { closePage(np->tab.index); }
};

}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_NOTEBOOK_H */

