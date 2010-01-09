#ifndef _LIBAMANITA_FILE_H
#define _LIBAMANITA_FILE_H

#include <stdio.h>
#include <time.h>


/** aFile is a class with static methods for handling files and directories.
 */
class aFile {
public:
	static const char *getHomeDir();

	static bool exists(const char *fn);
	static time_t accessed(const char *fn);
	static time_t modified(const char *fn);
	static long copy(const char *s,const char *d);
	static long copy(FILE *s,FILE *d);
	static bool remove(const char *dn);
	static bool mkdir(const char *dn,int p=0700);
};


#endif /* _LIBAMANITA_FILE_H */

