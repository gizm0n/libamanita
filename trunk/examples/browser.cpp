
#include <stdio.h>
#include <amanita/Application.h>
#include <amanita/tk/Browser.h>
#include <amanita/tk/Window.h>

using namespace a;
using namespace a::tk;


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
	Browser *browser;
public:
	BrowserApp(uint32_t params,const char *prj,const char *nm) : Application(params,prj,nm),browser(0) {}
	void create();
};


BrowserApp app(INIT_GUI,"Browser","Browser Example");


static uint32_t window_events(Widget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}

static uint32_t browser_events(Widget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return 0;
}


void BrowserApp::create() {
	Window *window;
	window = new Window(this,window_events);
	setMainWindow(window);
	window->setStyle(WINDOW_RESIZABLE,900,600);
	browser = new Browser(browser_events);
	browser->setStyle(FILL);
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

