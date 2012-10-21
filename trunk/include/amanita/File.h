#ifndef _AMANITA_FILE_H
#define _AMANITA_FILE_H

/**
 * @file amanita/File.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2010-01-09
 */ 

#include <stdio.h>
#include <time.h>


/** @cond */
#ifdef USE_UNIX
#define aFILE_DIRSEP "/"
#endif
#ifdef USE_WIN32
#define aFILE_DIRSEP "\\"
#endif
/** @endcond */


/** Amanita Namespace */
namespace a {


/** Check if file exists.
 * @param fn File name.
 * @return True if file exists, else false. */
extern bool exists(const char *fn);
/** Check when file was last accessed/read.
 * @param fn File name.
 * @return time_t value for last access. */
extern time_t accessed(const char *fn);
/** Check when file was last modified/changed.
 * @param fn File name.
 * @return time_t value for last modification. */
extern time_t modified(const char *fn);
/** Copy file s to d.
 * @param s File name to read.
 * @param s File name to copy to.
 * @return Number of bytes copied or -1 if fail. */
extern long copy(const char *s,const char *d);
/** Copy file s to d.
 * @param s File stream to read.
 * @param s File stream to copy to.
 * @return Number of bytes copied or -1 if fail. */
extern long copy(FILE *s,FILE *d);
/** Remove file or directory.
 * @param fn File name.
 * @return True if file was removed, else false. */
extern bool remove(const char *fn);
/** Create a directory.
 * @param dir Name of directory to create.
 * @param p Permissions, default 0700.
 * @return True if the directory was created, else false. */
extern bool mkdir(const char *dir,int p=0700);

/** Filename of the executable, that is the application calling the function.
 * @param fn The function writes file name to fn.
 * @param l Length in bytes of fn. */
extern void executable(char *fn,int l);
/** Home directory, on Linux this is ~/ on windows this is current directory.
 * @param dir The function writes directory to dir.
 * @param l Length in bytes of dir. */
extern void homedir(char *dir,int l);
/** Directory where executable is located.
 * @param dir The function writes directory to dir.
 * @param l Length in bytes of dir. */
extern void applicationdir(char *dir,int l);
/** Directory where fonts are installed.
 * @param dir The function writes directory to dir.
 * @param l Length in bytes of dir. */
extern void fontsdir(char *dir,int l);
/** System directory, on Linux this is home dir, for windows this is usually c:\windows.
 * @param dir The function writes directory to dir.
 * @param l Length in bytes of dir. */
extern void systemdir(char *dir,int l);
/** Current directory, from where application is called.
 * @param dir The function writes directory to dir.
 * @param l Length in bytes of dir. */
extern void currentdir(char *dir,int l);


/** A filehandling class.
 * Contains methods for handling the filesystem.
 * 
 * @ingroup amanita */
class File {
private:
	char *dir;
	char *name;
	FILE *fp;

public:
	static const char *dirsep;			//!< Separator for directories, eg. for Unix '/' and windows '\\'.

	File() : dir(0),name(0),fp(0) {}
	~File() { close(); }

	const char *getDirectory() { return dir; }
	const char *getName() { return name; }
	const FILE *getFile() { return fp; }

	File &open(const char *a,const char *fn, ...);
	File &close();
	File &read(char **data,size_t &len);
	File &write(const char *data,size_t len);
	size_t size();
};

}; /* namespace a */


#endif /* _AMANITA_FILE_H */

