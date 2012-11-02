
#include <stdio.h>
#include <amanita/Main.h>
#include <amanita/net/Http.h>

using namespace a;

int main(int argc, char *argv[]) {
	Main app(APP_SOCKETS);
	Http http;
	app.open(argc,argv);
	http.setUserAgent("Http Class User-Agent, v.0.1");

	http.get("www.google.com","");

	const char *fn = "http.txt";
	FILE *fp = fopen(fn,"wb");
	if(fp) {
		fwrite(http.getFile(),http.getFileSize(),1,fp);
		fclose(fp);
	} else perror(fn);
	return 0;
}

