#ifndef _AMANITA_FILE_H
#define _AMANITA_FILE_H

/**
 * @file amanita/aFile.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2010-01-09
 */ 

#include <stdio.h>
#include <time.h>


#if defined __linux__
#define FILE_DIRSEP "/"
#elif defined WIN32
#define FILE_DIRSEP "\\"
#else
#define FILE_DIRSEP "/"
#endif


/** A filehandling class.
 * 
 * Contains methods for handling the filesystem.
 * @ingroup amanita
 */
class aFile {
private:
	char *dir;
	char *name;
	FILE *file;

public:
	static bool exists(const char *fn);
	static time_t accessed(const char *fn);
	static time_t modified(const char *fn);
	static long copy(const char *s,const char *d);
	static long copy(FILE *s,FILE *d);
	static bool remove(const char *dn);
	static bool mkdir(const char *dn,int p=0700);

	aFile() : dir(0),name(0),file(0) {}
	~aFile() { close(); }

	const char *getDirectory() { return dir; }
	const char *getName() { return name; }
	const FILE *getFile() { return file; }

	aFile &open(const char *a,const char *fn, ...);
	aFile &close();
	aFile &read(char **data,size_t &len);
	aFile &write(const char *data,size_t len);
	size_t size();
};


#endif /* _AMANITA_FILE_H */

