#ifndef _AMANITA_APPLICATION_H
#define _AMANITA_APPLICATION_H

/**
 * @file amanita/Application.h
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-12-01
 */ 

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libintl.h>
#ifdef USE_GTK
#include <gtk/gtk.h>
#endif
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/Config.h>
#include <amanita/Thread.h>
//#include <amanita/tk/Window.h>



/** @cond */
#ifdef USE_WIN32
#ifdef main
#undef main
#endif
#define main amanita_main
extern int main(int, char *[]);
extern HINSTANCE hMainInstance;
#endif
/** @endcond */


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */


/** @cond */
namespace tk { class Window; };
/** @endcond */


/** Flags used when initiating applocation.
 * @see Application(uint32_t,const char *,const char *) */
enum {
	INIT_DIRECTORIES		= 0x00000001,	//!< Automatically manage directories, eg. ~/.project etc. Don't set this flag if you manage directories manually.
	INIT_LOG					= 0x00000002,	//!< Open a log-file for stdout and stderr.
	INIT_CONSOLE			= 0x00000010,	//!< Create a console application.
	INIT_WINDOWED			= 0x00000020,	//!< Create a windowed application.
	INIT_FULLSCREEN		= 0x00000040,	//!< Create a fullscreen application.
	INIT_GETTEXT			= 0x00000200,	//!<
	INIT_THREADS			= 0x00001000,	//!<
	INIT_SOCKETS			= 0x00002000,	//!<
	INIT_GUI					= 0x00010000,	//!<
	INIT_SDL					= 0x01000008,	//!<
	INIT_SDL_TTF			= 0x03000000,	//!<
	INIT_SDL_IMAGE			= 0x05000000,	//!<
};



/** Callback function to receive installation progress, i.e. for updating a progressbar.
 * @param obj The "obj" value used when calling install.
 * @param file Name of the file that has been installed.
 * @param n Index number of the file in the list.
 * @param max The highest number n can be.
 * @param st Status flags. 1=font(s) installed (on Linux call updateFontCache).
 * @see int install(const char *,const char *,Vector &,install_function,void *)
 * @see void updateFontCache() */
typedef void (*install_function)(void *obj,const char *file,int n,int max,int st);


/** A baseclass for applications.
 * 
 * Any application may use this class as a baseclass and inherit some main application
 * structure from it. It contains methods for setting up an environment, downloading
 * and installing files from a webserver. Loading and language files and to get textstrings,
 * supporting multilanguage applications etc.
 * 
 * @ingroup amanita */
class Application {
private:
	unsigned long app_init_params;	//!< 
	FILE *app_out;							//!< Output log-file, stdout and stderr is written to this file too.
	char *app_project;					//!< Project name.
	char *app_name;						//!< Application name, the name of this application. There can be many applications within the same project.
	char *app_user_agent;				//!< User Agent named in communication over http.
	char *app_home_dir;					//!< Home directory, on Linux ~/.prj where prj is the name of the project, .\ for windows.
	char *app_data_dir;					//!< Data directory, on Linux /usr/share/prj where prj is the name of the project, .\data for windows.
	char *app_locale_dir;				//!< Directory where locale files are stored.
	char *app_fonts_dir;					//!< Directory where font files are stored.
	Thread app_thread;					//!< The mutex is mainly used as a standard lock/unlock of the application.
	uint32_t app_local_id;				//!< A local ID for the running instance.
	time_t app_local_time;				//!<
	time_t app_last_access;				//!< Time the application last was executed.

protected:
	tk::Window *window;					//!< Main window for the application.

public:
	/** Constructor, initiates the application.
	 * It automatically looks for directories associated with the project, and 
	 * @param params Parameters for how to initiate application.
	 * @param prj Name of the project.
	 * @param nm Name of the application. */
	Application(uint32_t params=0,const char *prj=0,const char *nm=0);
	virtual ~Application();

	/** Initialize application, initialize resources. */
	uint32_t open(int argc,char *argv[]);
	uint32_t close();						//!< Close application, uninitialize and shut down the application, free resources.
	int main();								//!< Start main loop.
	void quit();							//!< Exit the main loop.

	void setMainWindow(tk::Window *wnd) { window = wnd; }
	bool isMainWindow(tk::Window *wnd) { return window==wnd; }
	tk::Window *getMainWindow() { return window; }

	virtual void create();				//!< Create aplication, this method should be inherited and used for creating the windows and widgets etc.
	virtual void destroy();				//!< Destroy application, inherit and use for freeing user created resources.

	int install(const char *host,const char *files,install_function func=0,void *obj=0);
/*
	int install(const char *host,const char *path,Vector &files,install_function func=0,void *obj=0);
*/
#if defined USE_GTK
	void updateFontCache();
#endif

	void lock() { app_thread.lock(); }
	void unlock() { app_thread.unlock(); }

	void setProjectName(const char *prj);
	const char *getProjectName() { return app_project; }
	void setApplicationName(const char *prj);
	const char *getApplicationName() { return app_name; }
	void setUserAgent(const char *ua);
	const char *getUserAgent() { return app_user_agent; }

	void setHomeDir(const char *dir);
	const char *getHomeDir() { return app_home_dir; }
	void setDataDir(const char *dir);
	const char *getDataDir() { return app_data_dir; }
	void setLocaleDir(const char *dir);
	const char *getLocaleDir() { return app_locale_dir; }
	void setFontsDir(const char *dir);
	const char *getFontsDir() { return app_fonts_dir; }

	void setLocalID(uint32_t id) { app_local_id = id; }
	uint32_t getLocalID() { return app_local_id; }
	void setLocalTime(time_t t) { app_local_time = t; }
	time_t getLocalTime() { return app_local_time; }
	time_t getLastAccess() { return app_last_access; }
};

}; /* namespace a */


#endif /* _AMANITA_APPLICATION_H */

