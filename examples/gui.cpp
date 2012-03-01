
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <cairo.h>
#include "../src/amanita/_config.h"
#include <amanita/aApplication.h>
#include <amanita/gui/aMenu.h>
#include <amanita/gui/aNotebook.h>
#include <amanita/gui/aStatus.h>
#include <amanita/gui/aWindow.h>
#include <amanita/gui/aBrowser.h>
#include <amanita/gui/aCanvas.h>
#include <amanita/gui/aCairo.h>
#include <amanita/gui/aContainer.h>
#include <amanita/gui/aButton.h>
#include <amanita/gui/aLabel.h>
#include <amanita/gui/aText.h>
#include <amanita/gui/aChoice.h>
#include <amanita/gui/aList.h>

enum {
	MENU_ID_FILE,
	MENU_ID_NEW,
	MENU_ID_SAVE,
	MENU_ID_LOAD,
	MENU_ID_RECENT,
	MENU_ID_FILE1,
	MENU_ID_FILE2,
	MENU_ID_FILE3,
	MENU_ID_FILE4,
	MENU_ID_PRINT,
	MENU_ID_QUIT,
	MENU_ID_HELP,
	MENU_ID_CONTENT,
	MENU_ID_ABOUT,
};


class GuiApp : public aApplication {
private:
	aNotebook *notebook;
public:
	GuiApp(const char *prj,const char *nm=0) : aApplication(prj,nm) {}
	virtual ~GuiApp() {}
	void create();
};


GuiApp app("GUI","aWidget Example");


static uint32_t window_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}

static uint32_t menu_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"menu_events(%s)\n",(const char *)p2);
	if(p1==MENU_ID_QUIT) {
		app.quit();
		return 1;
	}
	return 0;
}

static uint32_t notebook_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"notebook_events(%d,%s)\n",(int)e,(char *)p3);
	return 0;
}

static uint32_t canvas_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
//fprintf(stderr,"canvas_events(w: %p, e: %d, p1: %ld, p2: %ld, p3: %ld)\n",w,e,p1,p2,p3);
	if(e==aCANVAS_PAINT) {
		aCanvas *c = (aCanvas *)w;
		c->setColor(0xff0000);
		c->fillRectangle(0,0,c->getWidth(),c->getHeight());
		c->setColor(0x0000ff);
		c->drawRectangle(0,0,c->getWidth(),c->getHeight());
		return 1;
	}
	return 0;
}

static uint32_t cairo_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
//fprintf(stderr,"cairo_events(w: %p, e: %d, p1: %ld, p2: %ld, p3: %ld)\n",w,e,p1,p2,p3);
	if(e==aCAIRO_PAINT) {
		aCanvas *c = (aCanvas *)w;
		cairo_t *cr = (cairo_t *)p1;
		cairo_set_line_width(cr,1);
		cairo_set_source_rgb(cr,0.0,0.0,1.0); 
		cairo_rectangle(cr,0,0,c->getWidth(),c->getHeight());
		cairo_fill(cr);
		cairo_set_source_rgb(cr,1.0,0.0,0.0); 
		cairo_rectangle(cr,2,2,c->getWidth()-4,c->getHeight()-4);
		cairo_stroke(cr);
		return 1;
	}
	return 0;
}

static uint32_t button_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"button_events(%s, %d)\n",w->getText(),(int)p1);
	return 0;
}

static uint32_t choice_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"choice_events(%s)\n",((aChoice *)w)->getItem());
	return 0;
}

static uint32_t list_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"list_events(%s)\n",((aList *)w)->getColumn(0));
	return 0;
}


void GuiApp::create() {
	int i;
	aWindow *window;
	window = new aWindow(this,window_events);
	setMainWindow(window);
	window->setStyle(aWINDOW_RESIZABLE|aWINDOW_CENTER,900,600);

	const aMenuItem items[] = {
		{ -1,MENU_ID_FILE,"File",true,-1 },
			{ MENU_ID_FILE,MENU_ID_NEW,"New",true,aKEY_n,aKEY_CONTROL },
			{ MENU_ID_FILE,MENU_ID_SAVE,"Save",true,aKEY_s,aKEY_CONTROL },
			{ MENU_ID_FILE,MENU_ID_LOAD,"Load",true,aKEY_l,aKEY_CONTROL },
			{ MENU_ID_FILE,-1 },
			{ MENU_ID_FILE,MENU_ID_RECENT,"Recent Files",true,-1 },
			{ MENU_ID_RECENT,MENU_ID_FILE1,"File 1",true,-1 },
			{ MENU_ID_RECENT,MENU_ID_FILE2,"File 2",true,-1 },
			{ MENU_ID_RECENT,MENU_ID_FILE3,"File 3",true,-1 },
			{ MENU_ID_RECENT,MENU_ID_FILE4,"File 4",true,-1 },
			{ MENU_ID_FILE,-1 },
			{ MENU_ID_FILE,MENU_ID_PRINT,"Print",true,aKEY_p,aKEY_CONTROL },
			{ MENU_ID_FILE,-1 },
			{ MENU_ID_FILE,MENU_ID_QUIT,"Quit",true,aKEY_q,aKEY_CONTROL },
		{ -1,MENU_ID_HELP,"Help",true,-1 },
			{ MENU_ID_HELP,MENU_ID_CONTENT,"Content",true,aKEY_F1,0 },
			{ MENU_ID_HELP,-1 },
			{ MENU_ID_HELP,MENU_ID_ABOUT,"About",true,-1 },
		{ -1,-1 }
	};
	window->setMenu(new aMenu(menu_events,items));

	{
		const int cols[] = { 200,150,200,-1 };
		window->setStatus(new aStatus(cols));
	}

	notebook = new aNotebook(notebook_events);
	notebook->setStyle(aFILL|aNOTEBOOK_CLOSE_BUTTON,0,0,10);
fprintf(stderr,"window->add(notebook);\n");
	window->add(notebook);
	window->open(0);

	window->updateStatus(0,"aWidget Example");
	window->updateStatus(1,PACKAGE_STRING);
	window->updateStatus(2,"Per Löwgren 2012");

	aBrowser *browser = new aBrowser(0);
	browser->setStyle(aFILL);
	notebook->openPage("Browser Tab",browser);
	browser->setHtmlContent("<html>\n\
<body>\n\
<h1>Browser Page</h1>\n\
<p>Per L&ouml;wgren 2012</p>\n\
<a href=\"http://code.google.com/p/libamanita\">Amanita Library</a><br/>\n\
<a href=\"http://www.google.com\">Google</a><br/>\n\
<a href=\"javascript:alert('Javascript Works!');\">Javascript Test</a><br/>\n\
</body>\n\
</html>");

	aCairo *cairo = new aCairo(cairo_events);
	cairo->setStyle(aFILL);
	notebook->openPage("Cairo Tab",cairo);

	aCanvas *canvas = new aCanvas(canvas_events);
	canvas->setStyle(aFILL);
	notebook->openPage("Canvas Tab",canvas);

	int n = 0,s;
	aButton *button,*group;
	char str[32];
	aContainer *hbox1 = new aContainer();
	hbox1->setStyle(aHORIZONTAL|aEXPAND|aFILL,0,0,10,5);
	aContainer *hbox2 = new aContainer();
	hbox2->setStyle(aHORIZONTAL|aEXPAND|aFILL,0,0,0,5);
	aContainer *vbox1 = new aContainer();
	vbox1->setStyle(aVERTICAL|aFILL,0,0,0,5);
	vbox1->setText("Widgets");
	aContainer *vbox2 = new aContainer();
	vbox2->setStyle(aVERTICAL|aEXPAND|aFILL,0,0,0,5);

	for(i=0; i<5; ++i) {
		s = aFILL;
		if(i>=1 && i<=2) s = aFILL|aBUTTON_CHECKBOX;
		else if(i>=3 && i<=4) s = aFILL|aBUTTON_RADIOBUTTON;
		button = new aButton(button_events);
		if(i==3) group = button;
		else if(i>3) button->setGroup(group);
		button->setStyle(s);
		sprintf(str,"Button %d",++n);
		button->setText(str);
		vbox1->add(button);
	}
	aChoice *choice = new aChoice(choice_events);
	for(i=0; i<10; ++i) {
		sprintf(str,"Choice %d",i+1);
		choice->addItem(str);
	}
	choice->select(5);
	vbox1->add(choice);
	aLabel *label = new aLabel(0);
	label->setStyle(aFILL|aLABEL_LEFT);
	label->setText("Label");
	vbox1->add(label);
	aText *text = new aText(0);
	vbox1->add(text);
	choice = new aChoice(choice_events);
	choice->setStyle(aFILL|aCHOICE_ENTRY);
	for(i=0; i<10; ++i) {
		sprintf(str,"Choice Entry %d",i+1);
		choice->addItem(str);
	}
	vbox1->add(choice);
	hbox2->add(vbox1);

	aList *list = new aList(list_events);
	list->setStyle(aEXPAND|aFILL);
	{
		const char *cols = "Name|Number|Astrology";
		const int col_widths[] = { 150,50,100 };
		list->setColumns(cols,col_widths);
	}
	hbox2->add(list);
	vbox2->add(hbox2);

	button = new aButton(button_events);
	button->setStyle(aFILL|aBUTTON_DEFAULT);
	sprintf(str,"Button %d",++n);
	button->setText(str);
	vbox2->add(button);
	hbox1->add(vbox2);

	button = new aButton(button_events);
	button->setStyle(aFILL,200,0);
	sprintf(str,"Button %d",++n);
	button->setText(str);
	hbox1->add(button);

//	notebook->openPage("Widgets Tab",hbox1);

	aWindow *dialog = new aWindow(this,window_events);
	dialog->setStyle(aWINDOW_DIALOG|aWINDOW_CENTER,640,480);

	dialog->add(hbox1);
	dialog->open(window);

	const char *astro[] = { "☉♈","☽♉","☿♊","♀♋","♂♌","♃♍","♄♎","♅♏","♆♐","♇♑" };
	for(i=0; i<10; ++i) {
		sprintf(str,"List View %d|%d|%s",i+1,i+1,astro[i]);
		list->addRow(str);
	}
}


int main(int argc,char *argv[]) {
	int ret;
	aObject::printClasses(stdout);
	app.open(argc,argv,aINIT_GUI);
	ret = app.main();
	app.close();
	return ret;
}

