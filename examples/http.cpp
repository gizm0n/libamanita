
#include <stdio.h>
#include <amanita/aApplication.h>
#include <amanita/net/aHttp.h>

int main(int argc, char *argv[]) {
	aApplication app(aINIT_SOCKETS);
	aHttp http;
	app.open(argc,argv);
	http.setUserAgent("aHttp Class User-Agent, v.0.1");

	http.get("www.google.com","");

	FILE *fp = fopen("http.txt","w");
	fwrite(http.getFile(),http.getFileSize(),1,fp);
	fclose(fp);
}

