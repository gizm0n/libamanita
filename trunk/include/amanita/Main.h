#ifndef _AMANITA_MAIN_H
#define _AMANITA_MAIN_H

/**
 * @file amanita/Main.h
 * @author Per Löwgren
 * @date Modified: 2012-10-23
 * @date Created: 2008-12-01
 */ 

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libintl.h>
#include <amanita/Config.h>
#include <amanita/Thread.h>
//#include <amanita/tk/Window.h>

/** @cond */
#ifdef USE_WIN32
#ifdef main
#undef main
#endif
/* Define main as amanita_main to force the use of WinMain. */
#define main amanita_main
extern int main(int, char *[]);
extern HINSTANCE hMainInstance;
#endif
/** @endcond */


/** Amanita Namespace */
namespace a {


/** Flags used when initiating applocation.
 * @see Main(uint32_t,const char *,const char *) */
enum {
	APP_DIRECTORIES		= 0x00000001,	//!< Automatically manage directories, eg. ~/.project etc. Don't set this flag if you manage directories manually.
	APP_LOG					= 0x00000002,	//!< Open a log-file for stdout and stderr.
	APP_CONSOLE				= 0x00000010,	//!< Create a console application.
	APP_WINDOWED			= 0x00000020,	//!< Create a windowed application.
	APP_FULLSCREEN			= 0x00000040,	//!< Create a fullscreen application.
	APP_GETTEXT				= 0x00000100,	//!<
	APP_THREADS				= 0x00000200,	//!<
	APP_SOCKETS				= 0x00000400,	//!<
	APP_TK					= 0x00001000,	//!<
	APP_GUI					= 0x00ff0000,	//!<
	APP_SDL					= 0x00010000,	//!<
	APP_SDL_TTF				= 0x00030000,	//!<
	APP_SDL_IMG				= 0x00050000,	//!<
	APP_DD					= 0x00100000,	//!< Initialize DirectDraw
	APP_AUDIO				= 0x01000000,	//!<
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
class Main {
protected:
	uint32_t app_params;					//!< 
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

public:
	/** Constructor, initiates the application.
	 * It automatically looks for directories associated with the project, and 
	 * @param p Parameters for how to initiate application.
	 * @param prj Name of the project.
	 * @param nm Name of the application. */
	Main(uint32_t p=0,const char *prj=0,const char *nm=0);
	virtual ~Main();

	bool manageDirectories() { return app_params&APP_DIRECTORIES; }
	bool useLog() { return app_params&APP_LOG; }
	bool isConsole() { return app_params&APP_CONSOLE; }
	bool isWindowed() { return app_params&APP_WINDOWED; }
	bool isFullscreen() { return app_params&APP_FULLSCREEN; }
	bool useGettext() { return app_params&APP_GETTEXT; }
	bool useThreads() { return app_params&APP_THREADS; }
	bool useSockets() { return app_params&APP_SOCKETS; }
	bool useToolkit() { return app_params&APP_TK; }
	bool useGUI() { return app_params&APP_GUI; }
	bool useSDL() { return app_params&APP_SDL; }
	bool useDirectDraw() { return app_params&APP_DD; }
	bool useAudio() { return app_params&APP_AUDIO; }

	/** Initialize application, initialize resources. */
	virtual uint32_t open(int argc,char *argv[]);
	/** Close application, uninitialize and shut down the application, free resources. */
	virtual uint32_t close();

	int install(const char *host,const char *files,install_function func=0,void *obj=0);
/*
	int install(const char *host,const char *path,Vector &files,install_function func=0,void *obj=0);
*/

	void lock() { app_thread.lock(); }
	void unlock() { app_thread.unlock(); }

	void start(thread_function f=0);
	void stop();
	void kill();
	virtual void run();
	void pauseFPS(int fps) { app_thread.pauseFPS(fps); }
	void pause(int millis) { app_thread.pause(millis); }
	bool isRunning() { return app_thread.isRunning(); }

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


#endif /* _AMANITA_MAIN_H */

