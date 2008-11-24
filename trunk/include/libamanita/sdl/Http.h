#ifndef _LIBAMANITA_SDL_HTTP_H
#define _LIBAMANITA_SDL_HTTP_H

#include <libamanita/Hashtable.h>

enum HTTP_METHOD {
	HTTP_METHOD_HEADER,
	HTTP_METHOD_GET,
	HTTP_METHOD_POST,
};

enum HTTP_HEADER {
	HTTP_ACCEPT,
	HTTP_ACCEPT_CHARSET,
	HTTP_ACCEPT_ENCODING,
	HTTP_ACCEPT_LANGUAGE,
	HTTP_ACCEPT_RANGES,
	HTTP_AGE,
	HTTP_ALLOW,
	HTTP_AUTHORIZATION,
	HTTP_CACHE_CONTROL,
	HTTP_CONNECTION,
	HTTP_CONTENT_DISPOSITION,
	HTTP_CONTENT_ENCODING,
	HTTP_CONTENT_LANGUAGE,
	HTTP_CONTENT_LENGTH,
	HTTP_CONTENT_LOCATION,
	HTTP_CONTENT_MD5,
	HTTP_CONTENT_RANGE,
	HTTP_CONTENT_TYPE,
	HTTP_DATE,
	HTTP_EXPECT,
	HTTP_FROM,
	HTTP_HOST,
	HTTP_IF_MATCH,
	HTTP_IF_MODIFIED_SINCE,
	HTTP_IF_NONE_MATCH,
	HTTP_IF_RANGE,
	HTTP_IF_UNMODIFIED_SINCE,
	HTTP_LAST_MODIFIED,
	HTTP_LOCATION,
	HTTP_MAX_FORWARDS,
	HTTP_PROXY_AUTHORIZATION,
	HTTP_RANGE,
	HTTP_REFERER,
	HTTP_SERVER,
	HTTP_TE,
	HTTP_USER_AGENT,
};

enum HTTP_MIMES {
	HTTP_FORM_URLENCODED,
	HTTP_OCTET_STREAM,
	HTTP_MULTIPART_FORM_DATA,
	HTTP_MULTIPART_MIXED,
};


/** HTTP Protocol class.
 * This is a cute little class for communicating with a server through the HTTP protocol,
 * using the SDL and SDL_net libraries.
 * @author Per Löwgren
 * @date Created: 2008-04-09
 *
 * Code to demonstrate how this class can be used:
 * @code
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "sdl/Http.h"

int main(int argc, char *argv[]) {
	if(SDL_Init(0)<0) exit(1);
	if(SDLNet_Init()==-1) exit(2);

	Http http;
	http.setUserAgent("Http Class User-Agent, v.0.1");

	http.setFormValue("test1","test1");
	http.setFormValue("test2","test2=(test1&test2)");
	http.setFormFile("file","file.txt","text/plain",false);
	http.setFormFile("image","image.png","image/png",true);

	http.post("www.host.com","script.php");

	FILE *fp = fopen("response.txt","w");
	fwrite(http.getFile(),http.fileSize(),1,fp);
	fclose(fp);
	
	SDLNet_Quit();
	SDL_Quit();
}
 * @endcode
 */
class Http {
private:
	struct packet {
		char *file;
		char *content;
		bool loaded;
		bool binary;
		void *data;
		long len;
	};
	int timeout;				/**< Timeout for server responce																*/
	Hashtable headers;		/**< Request headers																				*/
	Hashtable form;			/**< Post form																						*/
	Hashtable response;		/**< Response headers																			*/
	int multipart;				/**< Is set if form data implies that it should be a multipart transfer.		*/
	float ver;					/**< Response HTTP version																		*/
	int status;					/**< Response status code																		*/
	String body;				/**< Response body data																			*/

public:
	Http();
	~Http();

	
	void setTimeout(int t) { timeout = t; } /**< Set timeout for request. */

	/** Set a request header.
	 * The header key should only be the string litteral exact name, without the ": ".
	 * @param key Header name.
	 * @param value Header value.
	 *
	 * @code
	 * Http http;
	 * http.setRequestHeader("Content-Type","text/html");
	 * @endcode
	 */
	void setRequestHeader(const char *key,const char *value);

	/** Set a request header.
	 * @see Http::setRequestHeader(const char *key,const char *value)
	 * @see HTTP_HEADER
	 */
	void setRequestHeader(HTTP_HEADER key,const char *value);

	/** Set the User-Agent header. */
	void setUserAgent(const char *agent) { setRequestHeader(HTTP_USER_AGENT,agent); }

	/** Set a form value.
	 * Neither the key nor the value should be URL encoded.
	 * @param key Name of the form-field.
	 * @param value The value.
	 * @param ... Use the va_list to format the formvalue.
	 */
	void setFormValue(const char *key,const char *value, ...);

	/** Send a file.
	 * This can either be a file on the harddrive or raw data. No need to set header for multipart-request, this is
	 * done when needed.
	 * @param key Name of the form-field.
	 * @param file Filename, the physical path to the file, or if data and len is set some name.
	 * @param content Content-Type MIME of the file.
	 * @param binary Set to true if the file should be handled as binary, false if it is text.
	 * @param data If set to NULL the program will load the file from harddrive.
	 * @param len Length of data.
	 */
	void setFormFile(const char *key,const char *file,const char *content,bool binary=true,void *data=NULL,unsigned long len=0);

	/** Remove a form value.
	 * @param key Same key that was used in setFormValue or setFormFile.
	 */
	void removeFormValue(const char *key);
	/** Clear the form. */
	void clearForm();


	/** Send a htt-prequest using the GET method.
	 * The va_list can be used to format the url with extra params. Note that params should be URL encoded.
	 * @param host A string containing the host.
	 * @param url The script or file to be requested.
	 * @param ... Optional values to be formatted into url.
	 * @return The response body data.
	 * @see String::encodeURL()
	 *
	 * @code
	 * Http http;
	 * http.get("www.host.com","script.php?param1=%s&param2=%d",param1,param2);
	 * @endcode
	 */
	const char *get(const char *host,const char *url, ...);

	/** Send a htt-prequest using the POST method.
	 * @param host A string containing the host.
	 * @param url The script or file to be requested.
	 * @return The response body data.
	 */
	const char *post(const char *host,const char *url);

	/** Send a htt-prequest using the POST method.
	 * This method does not make use of the values set with setFormValue() and setFormFile().
	 * @param host A string containing the host.
	 * @param url The script or file to be requested.
	 * @param form The form string formatted with '=' and '&'.
	 * @param ... Optional values to be formatted into form.
	 * @return The response body data.
	 * @see Http::get(const char *host,const char *url, ...)
	 *
	 * @code
	 * Http http;
	 * http.post("www.host.com","script.php","param1=%s&param2=%d",param1,param2);
	 * @endcode
	 */
	const char *post(const char *host,const char *url,const char *form, ...);

	/** Send a http-prequest using specified method.
	 * Set headers and form data previous to calling this method.
	 * @param host A string containing the host.
	 * @param url The script or file to be requested.
	 * @param method The http-method to be used.
	 * @param data The data that is sent with the request.
	 * @param len Length of the data sent with the request.
	 * @see HTTP_METHOD
	 *
	 * @code
	 * Http http;
	 * http.setFormValue("key","value");
	 * http.request("www.host.com","script.php",POST);
	 * @endcode
	 */
	const char *request(const char *host,const char *url,HTTP_METHOD method,const char *data=NULL,unsigned long len=0);

	/** Get the HTTP version returned by the response. */
	float getHTTPVersion() { return ver; }
	/** Get the resonse-status, 200 means OK, 404 means not found ets. */
	int getResponseStatus() { return status; }
	/** Get response header. */
	const char *getResponseHeader(const char *key);
	/** Get response header.
	 * @see HTTP_HEADER
	 */
	const char *getResponseHeader(HTTP_HEADER key);
	/** Get the file-body returned by the get, post or request methods. */
	const char *getFile() { return body.toString(); }
	/** Get size of the file-body returned by the get, post or request methods. */
	long getFileSize() { return body.length(); }
};


#endif /* _LIBAMANITA_SDL_HTTP_H */

