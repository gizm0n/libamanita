
#include <stdio.h>
#include <amanita/Application.h>
#include <amanita/gui/aBrowser.h>
#include <amanita/gui/aWindow.h>


const char *html_document = "\
<html>\n\
<body>\n\
<h1>Hello World!</h1>\n\
<p>Per L&ouml;wgren 2012</p>\n\
<a href=\"http://code.google.com/p/libamanita\">Amanita Library</a><br/>\n\
<a href=\"http://www.google.com\">Google</a><br/>\n\
<a href=\"javascript:alert('Javascript Works!');\">Javascript Test</a><br/>\n\
</body>\n\
</html>\n";


class BrowserApp : public Application {
private:
	aBrowser *browser;
public:
	BrowserApp(uint32_t params,const char *prj,const char *nm) : Application(params,prj,nm),browser(0) {}
	void create();
};


BrowserApp app(INIT_GUI,"Browser","aBrowser Example");


static uint32_t window_events(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}

static uint32_t browser_events(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}


void BrowserApp::create() {
	aWindow *window;
	window = new aWindow(this,window_events);
	setMainWindow(window);
	window->setStyle(aWINDOW_RESIZABLE,900,600);
	browser = new aBrowser(browser_events);
	browser->setStyle(aFILL);
	window->add(browser);
	window->open(0);
	browser->setHtmlContent(html_document);
}

int main(int argc,char *argv[]) {
	int ret;
	app.open(argc,argv);
	ret = app.main();
	app.close();
	return ret;
}

