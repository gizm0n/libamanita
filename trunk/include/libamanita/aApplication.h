#ifndef _LIBAMANITA_APPLICATION_H
#define _LIBAMANITA_APPLICATION_H

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libamanita/aHashtable.h>


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
		APP_LANG,
	};

protected:
	FILE *app_out;
	char *app_name;
	char *app_agent;
	uint32_t app_local_id;
	time_t app_local_time;
	time_t app_last_access;
	char app_lang[3];
	aHashtable app_lang_data;
	aHashtable app_properties;

public:
	aApplication(const char *nm);
	virtual ~aApplication();

	const char *getApplicationName() { return app_name; }
	void setAgent(const char *a) { if(app_agent && app_agent!=app_name) free(app_agent);app_agent = strdup(a); }
	const char *getAgent() { return app_agent; }

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

	int install(const char *host,const char *path,const char *files[]);

	void loadProperties();
	void saveProperties();
	const char *getProperty(const char *key) { return (const char *)app_properties.get(key); }
	void setProperty(const char *key,const char *value) { app_properties.put(key,value); }

	virtual void setLanguage(const char *l);
	const char *getLanguage() { return app_lang; }
	const char *getf(const char *format, ...);
	const char *get(const char *key);

	void printf(const char *format, ...);
};


#endif /* _LIBAMANITA_APPLICATION_H */

