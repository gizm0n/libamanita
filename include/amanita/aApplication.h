#ifndef _AMANITA_APPLICATION_H
#define _AMANITA_APPLICATION_H

/**
 * @file amanita/aApplication.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-12-01
 */ 

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libintl.h>
#include <amanita/aThread.h>


class aWindow;

#ifdef __linux__
#include <gtk/gtk.h>



#elif defined(WIN32)
#include <windows.h>

#ifdef main
#undef main
#endif
#define main amanita_main
extern int main(int, char *[]);
#endif


enum {
	AMANITA_INIT_CONSOLE			= 0x00000001,	//!<
	AMANITA_INIT_WINDOWED		= 0x00000002,	//!<
	AMANITA_INIT_FULLSCREEN		= 0x00000004,	//!<
	AMANITA_INIT_DIRECTORIES	= 0x00000010,	//!<
	AMANITA_INIT_GETTEXT			= 0x00000020,	//!<
	AMANITA_INIT_THREADS			= 0x00000100,	//!<
	AMANITA_INIT_SOCKETS			= 0x00000200,	//!<
	AMANITA_INIT_GUI				= 0x00001000,	//!<
	AMANITA_INIT_SDL				= 0x00100008,	//!<
	AMANITA_INIT_SDL_TTF			= 0x00300000,	//!<
	AMANITA_INIT_SDL_IMAGE		= 0x00500000,	//!<
};


#define _(str) gettext(str)


/** Callback function to receive installation progress, i.e. for updating a progressbar.
 * @param obj The "obj" value used when calling install.
 * @param file Name of the file that has been installed.
 * @param n Index number of the file in the list.
 * @param max The highest number n can be.
 * @param st Status flags. 1=font(s) installed (on Linux call updateFontCache).
 * @see int nstall(const char *,const char *,aVector &,install_function,void *)
 * @see void updateFontCache()
 */
typedef void (*install_function)(void *obj,const char *file,int n,int max,int st);


/** A baseclass for applications.
 * 
 * Any application may use this class as a baseclass and inherit some main application
 * structure from it. It contains methods for setting up an environment, downloading
 * and installing files from a webserver. Loading and language files and to get textstrings,
 * supporting multilanguage applications etc.
 * @ingroup amanita
 */
class aApplication {
private:
	unsigned long app_init_params;	//!< 
	FILE *app_out;							//!< Output log-file, stdout and stderr is written to this file too.
	char *app_project;					//!< Project name.
	char *app_name;						//!< Application name, the name of this application. There can be many applications within the same project.
	char *app_user_agent;				//!< User Agent named in communication over http.
	char *app_locale_dir;				//!< Directory where locale files are stored.
	aThread app_thread;					//!< The mutex is mainly used as a standard lock/unlock of the application.
	uint32_t app_local_id;				//!< A local ID for the running instance.
	time_t app_local_time;				//!<
	time_t app_last_access;				//!< Time the application last was executed.

	aWindow *window;						//!< Main window for the application.

protected:
#ifdef WIN32
	HINSTANCE hinst;
#endif

public:
	aApplication();
	aApplication(const char *prj,const char *nm);
	virtual ~aApplication();

	/** Initialize application, initialize resources. */
	uint32_t open(int argc,char *argv[],uint32_t params);
	uint32_t close();						//!< Close application, uninitialize and shut down the application, free resources.
	int main();							//!< Start main loop.
	void quit();							//!< Exit the main loop.

	void setMainWindow(aWindow *wnd) { window = wnd; }
	bool isMainWindow(aWindow *wnd) { return window==wnd; }
	aWindow *getMainWindow() { return window; }

	virtual int create();				//!< Create aplication, this method should be inherited and used for creating the windows and widgets etc.
	virtual int destroy();				//!< Destroy application, inherit and use for freeing user created resources.

/*
	int install(const char *host,const char *path,aVector &files,install_function func=0,void *obj=0);
#if defined __linux__
	void updateFontCache();
#endif
*/

	void lock() { app_thread.lock(); }
	void unlock() { app_thread.unlock(); }

	void setProjectName(const char *prj);
	const char *getProjectName() { return app_project; }
	void setApplicationName(const char *prj);
	const char *getApplicationName() { return app_name; }
	void setUserAgent(const char *ua);
	const char *getUserAgent() { return app_user_agent; }

	void setDirectories(const char *dir);
	const char *getLocaleDir() { return app_locale_dir; }

	void setLocalID(uint32_t id) { app_local_id = id; }
	uint32_t getLocalID() { return app_local_id; }
	void setLocalTime(time_t t) { app_local_time = t; }
	time_t getLocalTime() { return app_local_time; }
	time_t getLastAccess() { return app_last_access; }
};


#endif /* _AMANITA_APPLICATION_H */

