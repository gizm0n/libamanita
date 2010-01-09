#ifndef _LIBAMANITA_APPLICATION_H
#define _LIBAMANITA_APPLICATION_H

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <libamanita/aHashtable.h>


class aApplication {
protected:
	FILE *out;
	char *name;
	char *agent;
	uint32_t local_id;
	time_t local_time;
	time_t last_access;
	char lang[3];
	aHashtable _lang;

public:
	aHashtable settings;

	struct {
		char home[256];
		char fonts[256];
		char lang[256];
	} dir;

	aApplication(const char *nm);
	virtual ~aApplication();

	void setAgent(const char *a) { if(agent && agent!=name) free(agent);agent = strdup(a); }
	void setLocalID(uint32_t id) { local_id = id; }
	void setLocalTime(time_t t) { local_time = t; }

	int install(const char *host,const char *path,const char *files[]);

	void loadSettings();
	void saveSettings();
	virtual void setLanguage(const char *l);
	const char *getf(const char *format, ...);
	const char *get(const char *key);

	void printf(const char *format, ...);
};


#endif /* _LIBAMANITA_APPLICATION_H */

