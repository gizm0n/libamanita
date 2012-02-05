#ifndef _AMANITA_THREAD_H
#define _AMANITA_THREAD_H

/**
 * @file amanita/aThread.h
 * @author Per Löwgren
 * @date Modified: 2012-02-04
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/aConfig.h>

/*#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_thread.h>
#elif defined(__linux__)*/
#ifdef AMANITA_PTHREADS
	#include <pthread.h>
	#include <unistd.h>
	#include <sys/time.h>
#elif defined(AMANITA_WIN32_THREADS)
	#include <windows.h>
	#include <sys/time.h>
#endif


typedef void (*thread_function)(void *);


/** A generic thread class.
 * @ingroup amanita
 */
class aThread {
private:
/*#ifdef LIBAMANITA_SDL
	SDL_Thread *thread;
	SDL_mutex *mutex;
	uint32_t timer;
#elif defined(__linux__)*/
#ifdef AMANITA_PTHREADS
	pthread_t *thread;
	pthread_mutex_t *mutex;
	timeval time;
#elif defined(AMANITA_WIN32_THREADS)
	HANDLE thread;
	HANDLE mutex;
	timeval time;
#endif
	thread_function function;
	void *data;

/*#ifdef LIBAMANITA_SDL
	static int _run(void *d);
#elif defined(__linux__)*/
#ifdef AMANITA_PTHREADS
	static void *_run(void *d);
#elif defined(AMANITA_WIN32_THREADS)
	static DWORD WINAPI _run(void *d);
#endif

	void createMutex();
	void deleteMutex();

public:
	aThread();
	~aThread();

	void start(thread_function f,void *d=0);
	void stop();
	void kill();
	bool isRunning() { return thread!=0; }

	void lock();
	void unlock();

	void pauseFPS(int fps);
	void pause(int millis) {
/*#ifdef LIBAMANITA_SDL
		SDL_Delay(millis);
#elif defined(__linux__)*/
#ifdef AMANITA_PTHREADS
		usleep(millis*1000);
#elif defined(AMANITA_WIN32_THREADS)
		Sleep(millis);
#endif
	}
};

#endif /* _AMANITA_THREAD_H */
