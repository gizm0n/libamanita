#ifndef _LIBAMANITA_APPLICATION_H
#define _LIBAMANITA_APPLICATION_H

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aThread.h>


class aApplication {
private:
	void getExecutable(char *dir,int l);
	void getHomeDir(char *dir,int l);
	void getApplicationDir(char *dir,int l);
	void getFontsDir(char *dir,int l);
	void getSystemDir(char *dir,int l);
	void getCurrentDir(char *dir,int l);

	static const char *property_key[];
	enum {
		APP_EXE,
		APP_DIR_HOME,
		APP_DIR_DATA,
		APP_DIR_FONTS,
		APP_DIR_LANG,
		APP_TEMP_PROPERTIES,
		APP_LANG,
		APP_PROPERTIES,
	};

protected:
	FILE *app_out;							//!< Output log-file, stdout and stderr is written to this file too.
	char *app_project;					//!< Project name.
	char *app_name;						//!< Application name, the name of this application. There can be many applications within the same project.
	char *app_user_agent;				//!< User Agent named in communication over http.
	aThread app_thread;					//!< The mutex is mainly used as a standard lock/unlock of the application.
	uint32_t app_local_id;				//!< A local ID for the running instance.
	time_t app_local_time;				//!<
	time_t app_last_access;				//!< Time the application last was executed.
	char app_lang[3];						//!< Two-letter code for language used in the application.
	aHashtable app_lang_data;			//!< Hashtable containing textstrings in the loaded language.
	aHashtable app_properties;			//!< Hashtable containing configuration and settings property values.

	virtual void init();
	virtual void exit();

	int install(const char *host,const char *path,const char *files[]);

	void loadProperties();
	void saveProperties();

public:
	aApplication(const char *prj,const char *nm=0);
	virtual ~aApplication();

	void lock() { app_thread.lock(); }
	void unlock() { app_thread.unlock(); }

	const char *getProjectName() { return app_project; }
	const char *getApplicationName() { return app_name; }
	void setUserAgent(const char *a);
	const char *getUserAgent() { return app_user_agent; }

	void setLocalID(uint32_t id) { app_local_id = id; }
	uint32_t getLocalID() { return app_local_id; }
	void setLocalTime(time_t t) { app_local_time = t; }
	time_t getLocalTime() { return app_local_time; }
	time_t getLastAccess() { return app_last_access; }

	const char *getExecutable() { return getProperty(property_key[APP_EXE]); }
	const char *getHomeDir() { return getProperty(property_key[APP_DIR_HOME]); }
	const char *getDataDir() { return getProperty(property_key[APP_DIR_DATA]); }
	const char *getFontsDir() { return getProperty(property_key[APP_DIR_FONTS]); }
	const char *getLanguageDir() { return getProperty(property_key[APP_DIR_LANG]); }

	void setProperties(const char *s) { app_properties.merge(s); }
	const char *getProperty(const char *key) { return (const char *)app_properties.get(key); }
	void setProperty(const char *key,const char *value) { app_properties.put(key,value); }

	virtual void setLanguage(const char *l);
	const char *getLanguage() { return app_lang; }
	const char *getf(const char *format, ...);
	const char *get(const char *key);

	void printf(const char *format, ...);
};


#endif /* _LIBAMANITA_APPLICATION_H */

