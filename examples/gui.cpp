
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
#include <amanita/gui/aSelect.h>
#include <amanita/gui/aList.h>
#include <amanita/gui/aPanel.h>

//#ifdef USE_GTK
//#include "../icons/16x16/amanita.xpm"
//#endif
#ifdef USE_WIN32
#include "_resource.h"
#endif

#ifdef USE_SCINTILLA
#include <SciLexer.h>
#endif

static const char *html_page = "<html>\n\
<body>\n\
<h1>Browser Page</h1>\n\
<p>Per L&ouml;wgren 2012</p>\n\
<a href=\"http://code.google.com/p/libamanita\">Amanita Library</a><br/>\n\
<a href=\"http://www.google.com\">Google</a><br/>\n\
<a href=\"javascript:alert('Javascript Works!');\">Javascript Test</a><br/>\n\
</body>\n\
</html>";

static const char *wiki_page = "\
==Header 1==\n\
===Header 2===\n\
\n\
Paragraph...:D\n\
\n\
----\n\
\n\
Paragraph [[link]] text '''bold''' text ''italic'' and '''''bold italic'''''.\n\
\n\
===Header 3===\n\
====Header 4====\n\
Paragraph...\n\
http://www.mantichora.net/index.php\n\
text [b]bbcode[/b] text\n\
 Pre text ;) text text\n\
 pre pre pre pre\n\
 text text text.\n\
text, end of paragraph.\n\
\n\
Testing template {{hebrew|RShITh HGLGLIM}} text {{astro|aries|taurus|gemini|cancer|leo|virgo|libra|scorpio|sagittarius|capricorn|aquarius|pisces|sun|moon|mercury|venus|mars|jupiter|saturn|uranus|neptune|pluto}} text.\n\
\n\
[[File:Manticore.jpg|120px|thumb|Image of '''[[Manticore]]''' and companion]]\n\
\n\
<div style=\"color:red\">Div div div.</div>\n\
\n\
:A colon at the start of a line\n\
::causes the line to be indented,\n\
:::most commonly used on Talk pages.\n\
* ''Unordered lists'' are easy to do:\n\
** Start every line with a star.\n\
*** More stars indicate a deeper level.\n\
**: Previous item continues.\n\
** A new line\n\
* in a list\n\
marks the end of the list.\n\
* Of course you can start again.\n\
# ''Numbered lists'' are:\n\
## Very organized\n\
## Easy to follow\n\
#: Previous item continues\n\
A new line marks the end of the list.\n\
# New numbering starts with 1.\n\
\n\
{| style=\"background: yellow; color: green\"\n\
|-\n\
| abc || def || ghi\n\
|- style=\"background: red; color: white\"\n\
| jkl || mno || pqr\n\
|-\n\
| stu || style=\"background: silver\" | vwx || yz\n\
|}\n\
\n\
{| border=\"1\" cellpadding=\"5\" cellspacing=\"0\" align=\"center\"\n\
|+ '''An example table'''\n\
|-\n\
! style=\"background: #efefef;\" | First header\n\
! colspan=\"2\" style=\"background: #ffdead;\" | Second header\n\
|-\n\
| upper left\n\
| &nbsp;\n\
| rowspan=\"2\" style=\"border-bottom: 3px solid grey;\" valign=\"top\" |\n\
right side\n\
|-\n\
| style=\"border-bottom: 3px solid grey;\" | lower left\n\
| style=\"border-bottom: 3px solid grey;\" | lower middle\n\
|-\n\
| colspan=\"3\" align=\"center\" |\n\
{| border=\"0\"\n\
|+ ''A table in a table''\n\
|-\n\
| align=\"center\" width=\"150\" | [[File:Wiki.png]]\n\
| align=\"center\" width=\"150\" | [[File:Wiki.png]]\n\
|-\n\
| align=\"center\" colspan=\"2\" style=\"border-top: 1px solid red;<!--\n\
  --> border-right: 1px solid red; border-bottom: 2px solid red;<!--\n\
  --> border-left: 1px solid red;\" |\n\
Two Wikipedia logos\n\
|}\n\
|}\n\
<nowiki>\n\
The nowiki tag ignores [[wiki]]\n\
''markup''. It reformats text by\n\
removing newlines and multiple\n\
spaces. It still interprets\n\
special characters: &rarr;\n\
</nowiki>\n\
\n\
<pre>\n\
The <pre> tag ignores [[wiki]]\n\
''markup'' as does the <nowiki>\n\
tag. Additionally, <pre> displays\n\
in a mono-spaced font, but does\n\
not  reformat    text    spaces.\n\
It still interprets special\n\
characters: &rarr;\n\
</pre>\n\
\n\
Single square brackets holding\n\
[text without a http URL] are\n\
preserved, but single square\n\
brackets containing a URL are\n\
treated as being an external\n\
[http://www.google.com weblink].\n\
";

enum {
	MENU_ID_FILE,
	MENU_ID_NEW,
	MENU_ID_SAVE,
	MENU_ID_LOAD,
	MENU_ID_PAGE,
	MENU_ID_PAGE1,
	MENU_ID_PAGE2,
	MENU_ID_PAGE3,
	MENU_ID_PAGE4,
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
	GuiApp(uint32_t params,const char *prj,const char *nm) : aApplication(params,prj,nm) {}
	virtual ~GuiApp() {}
	void create();
	void selectPage(int n) { notebook->selectPage(n); }
};


GuiApp app(aINIT_GUI,"GUI","aWidget Example");


static uint32_t window_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}

static uint32_t menu_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"menu_events(%s)\n",(const char *)p2);
fflush(stderr);
	if(p1>=MENU_ID_PAGE1 && p1<=MENU_ID_PAGE4) app.selectPage(p1-MENU_ID_PAGE1);
	else if(p1==MENU_ID_QUIT) {
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
fprintf(stderr,"choice_events(%s)\n",((aSelect *)w)->getItem());
	return 0;
}

static uint32_t list_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"list_events(%s)\n",((aList *)w)->getColumn(0));
	return 0;
}

static uint32_t text_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"text_events()\n");
	return 0;
}

static uint32_t panel_events(aWidget *w,uint32_t e,intptr_t p1,intptr_t p2,intptr_t p3) {
fprintf(stderr,"panel_events(p1: %d, p2: %d)\n",(int)p1,(int)p2);
fflush(stderr);
	return 0;
}


void GuiApp::create() {
	int i,n,s;
	char str[1025];
	aWindow *window;
	aBrowser *browser;
	aCairo *cairo;
	aCanvas *canvas;
/*	aWindow *dialog;
	aContainer *hbox1;
	aContainer *hbox2;*/
	aContainer *vbox1;
/*	aContainer *vbox2;
	aButton *button,*group;
	aLabel *label;
	aSelect *choice;
	aList *list;*/
	aText *text;
	aPanel *panel;

	window = new aWindow(this,window_events);
	setMainWindow(window);
	window->setStyle(aWINDOW_RESIZABLE|aWINDOW_CENTER,900,600);
	{
		const aWindowIcon icons[] = {
#ifdef USE_GTK
//			{ 16,0,amanita_xpm },
			{ 16,0,0,"../icons/16x16/amanita.png" },
			{ 48,0,0,"../icons/48x48/amanita.png" },
#endif
#ifdef USE_WIN32
			{ 16,aAPP_ICON },
			{ 48,aAPP_ICON },
#endif
		{0}};
		window->setIcons(icons);

		const aMenuItem items[] = {
			{ -1,MENU_ID_FILE,"File",true,-1 },
				{ MENU_ID_FILE,MENU_ID_NEW,"New",true,aKEY_n,aKEY_CONTROL },
				{ MENU_ID_FILE,MENU_ID_SAVE,"Save",true,aKEY_s,aKEY_CONTROL },
				{ MENU_ID_FILE,MENU_ID_LOAD,"Load",true,aKEY_l,aKEY_CONTROL },
				{ MENU_ID_FILE,-1 },
				{ MENU_ID_FILE,MENU_ID_PAGE,"Select Page",true,-1 },
				{ MENU_ID_PAGE,MENU_ID_PAGE1,"Page 1",true,-1 },
				{ MENU_ID_PAGE,MENU_ID_PAGE2,"Page 2",true,-1 },
				{ MENU_ID_PAGE,MENU_ID_PAGE3,"Page 3",true,-1 },
				{ MENU_ID_PAGE,MENU_ID_PAGE4,"Page 4",true,-1 },
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

		const int cols[] = { 200,150,200,-1 };
		window->setStatus(new aStatus(cols));
	}

	notebook = new aNotebook(notebook_events);
	notebook->setStyle(aFILL|/*aNOTEBOOK_CLOSE_BUTTON|*/aNOTEBOOK_HIDE_TABS);
fprintf(stderr,"window->add(notebook);\n");
	window->add(notebook);
	window->open(0);

	window->updateStatus(0,"aWidget Example");
	window->updateStatus(1,PACKAGE_STRING);
	window->updateStatus(2,"Per Löwgren 2012");

	browser = new aBrowser(0);
	browser->setStyle(aFILL);
	notebook->openPage("Browser Tab",browser);
	browser->setHtmlContent(html_page);

	cairo = new aCairo(cairo_events);
	cairo->setStyle(aFILL);
	notebook->openPage("Cairo Tab",cairo);

	canvas = new aCanvas(canvas_events);
	canvas->setStyle(aFILL);
	notebook->openPage("Canvas Tab",canvas);

/*	hbox1 = new aContainer();
	hbox1->setStyle(aHORIZONTAL|aEXPAND|aFILL,0,0,10,5);
	hbox2 = new aContainer();
	hbox2->setStyle(aHORIZONTAL|aEXPAND|aFILL,0,0,0,5);
	vbox1 = new aContainer();
	vbox1->setStyle(aVERTICAL|aFILL,0,0,0,5);
	vbox1->setText("Widgets");
	vbox2 = new aContainer();
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
	choice = new aSelect(choice_events);
	for(i=0; i<10; ++i) {
		sprintf(str,"Choice %d",i+1);
		choice->addItem(str);
	}
	choice->select(5);
	vbox1->add(choice);
	label = new aLabel(0);
	label->setStyle(aFILL|aLABEL_LEFT);
	label->setText("Label");
	vbox1->add(label);
	text = new aText(0);
	sprintf(str,"Text %d",++n);
	text->setText(str);
	vbox1->add(text);
	choice = new aSelect(choice_events);
	choice->setStyle(aFILL|aSELECT_ENTRY);
	for(i=0; i<10; ++i) {
		sprintf(str,"Choice Entry %d",i+1);
		choice->addItem(str);
	}
	vbox1->add(choice);
	hbox2->add(vbox1);

	list = new aList(list_events);
	list->setStyle(aEXPAND|aFILL);
	{
		const char *cols = "Name|Number|Astrology";
		const int col_widths[] = { 100,40,100 };
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

	text = new aText(text_events);
	text->setStyle(aFILL|aTEXT_MULTILINE,240,0);
	sprintf(str,"Text %d",++n);
	text->setText(str);
	hbox1->add(text);

//	notebook->openPage("Widgets Tab",hbox1);

	dialog = new aWindow(this,window_events);
	dialog->setStyle(aWINDOW_DIALOG|aWINDOW_CENTER,640,480);

	dialog->add(hbox1);
	dialog->open(window);

	button->setFont("Times New Roman",20,aFONT_BOLD);*/

	vbox1 = new aContainer();
	vbox1->setStyle(aVERTICAL|aFILL|aEXPAND);

	{
		aPanelButton buttons[] = {
			{ aPANEL_BUTTON,aPANEL_NEW,1,0,0,"New Page Tooltip",0 },
			{ aPANEL_BUTTON,aPANEL_OPEN,2,0,0,"Open Page Tooltip",0 },
			{ aPANEL_BUTTON,aPANEL_SAVE,3,0,0,"Save Page Tooltip",0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_COPY,4,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_CUT,5,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_PASTE,6,0,0,0,0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_FIND,7,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_FIND_AND_REPLACE,8,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_DELETE,9,0,0,0,0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_UNDO,10,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_REDO,11,0,0,0,0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_PRINT_PREVIEW,12,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_PRINT,13,0,0,0,0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_CHECK,aPANEL_BOLD,12,0,0,0,0 },
			{ aPANEL_CHECK,aPANEL_ITALIC,12,0,0,0,0 },
			{ aPANEL_CHECK,aPANEL_UNDERLINE,12,0,0,0,0 },
			{ aPANEL_CHECK,aPANEL_STRIKE,12,0,0,0,0 },
			{ aPANEL_SEPARATOR,0,0,0,0,0,0 },
			{ aPANEL_BUTTON,aPANEL_ZOOM_IN,12,0,"Zoom In","Zoom In Tooltip",0 },
		{0}};
		panel = new aPanel(panel_events,buttons);
		panel->setStyle(aFILL);
	}
//	panel->setButtonImage(PANEL_BITMAP);
	vbox1->add(panel);
	text = new aText(text_events);
	text->setStyle(aFILL|aEXPAND|aTEXT_SOURCE);
	vbox1->add(text);
	notebook->openPage("Source Tab",vbox1);

	text->setFont("DejaVu Sans Mono",10);
	text->setTabWidth(3);
	text->setText(wiki_page);
#ifdef USE_SCINTILLA
	{
		aScintilla sci = text->getScintilla();
		scintilla_function sci_func = text->getScintillaFunction();
		sci_func(sci,SCI_STYLECLEARALL,0,0);
		sci_func(sci,SCI_SETLEXER,SCLEX_MEDIAWIKI,0);
		sci_func(sci,SCI_SETKEYWORDS,0,(sptr_t)"int char");
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_COMMENT,0x009900);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_HEADING,0x990000);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_HEADING,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_BOLD,0x003366);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_BOLD,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_ITALIC,0x003366);
		sci_func(sci,SCI_STYLESETITALIC,SCE_MEDIAWIKI_ITALIC,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_BOLDITALIC,0x003366);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_BOLDITALIC,1);
		sci_func(sci,SCI_STYLESETITALIC,SCE_MEDIAWIKI_BOLDITALIC,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_LIST,0x006633);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_LIST,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_TABLE,0x000099);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_TABLE,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_LINK,0x009900);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_LINK,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_TEMPLATE,0x996600);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_TEMPLATE,1);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_PARAM,0x003399);
		sci_func(sci,SCI_STYLESETFORE,SCE_MEDIAWIKI_SEPARATOR,0x6633009);
		sci_func(sci,SCI_STYLESETBOLD,SCE_MEDIAWIKI_SEPARATOR,1);
		n = sci_func(sci,SCI_TEXTWIDTH,STYLE_LINENUMBER,(sptr_t)"1234");
		sci_func(sci,SCI_SETMARGINWIDTHN,0,n);
		sci_func(sci,SCI_SETMARGINWIDTHN,1,0);
		sci_func(sci,SCI_SETMARGINWIDTHN,2,0);
	}
#endif
#ifdef USE_SOURCEVIEW
	{
		GtkWidget *editor = text->getEditor();
		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
		GtkSourceLanguageManager *lang_man = gtk_source_language_manager_new();
//		const char * const *arr = gtk_source_language_manager_get_language_ids(lang_man);
//		for(int i=0; arr[i]; ++i)
//			debug_output("aText::create(id[%d]: %s)\n",i,arr[i]);
		GtkSourceLanguage *lang = gtk_source_language_manager_get_language(lang_man,"c");
		gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer),lang);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor),GTK_WRAP_WORD_CHAR);
		gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(editor),TRUE);
		gtk_source_view_set_indent_on_tab(GTK_SOURCE_VIEW(editor),TRUE);
//		gtk_source_view_set_indent_width(GTK_SOURCE_VIEW (editor),2);
		gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(editor),TRUE);
		gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(editor),TRUE);
		gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(editor),FALSE);
//		gtk_source_view_set_right_margin_position(GTK_SOURCE_VIEW (editor),80);
	}
#endif

/*	const char *alpha[] = { "abc","def","ghi","jkl","mno","pqr","stu","vwx","yz","." };
	for(i=0; i<10; ++i) {
		sprintf(str,"List View %d|%d|%s",i+1,i+1,alpha[i]);
		list->addRow(str);
	}*/
}


int main(int argc,char *argv[]) {
	int ret;
	aObject::printClasses(stdout);
	app.open(argc,argv);
	ret = app.main();
	app.close();
	return ret;
}

