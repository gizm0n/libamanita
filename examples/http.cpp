
#include <stdio.h>
#include <amanita/net/aHttp.h>

int main(int argc, char *argv[]) {
	aHttp http;
	InitNetwork();
	http.setUserAgent("aHttp Class User-Agent, v.0.1");

	http.get("www.google.com","");

	FILE *fp = fopen("http.txt","w");
	fwrite(http.getFile(),http.getFileSize(),1,fp);
	fclose(fp);
	UninitNetwork();
}

