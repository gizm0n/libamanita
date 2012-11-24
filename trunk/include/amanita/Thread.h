#ifndef _AMANITA_THREAD_H
#define _AMANITA_THREAD_H

/**
 * @file amanita/Thread.h
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/Config.h>

/*#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>*/
#ifdef USE_PTHREADS
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef USE_WIN32_THREADS
#include <windows.h>
#include <sys/time.h>
#endif


/** Amanita Namespace */
namespace a {

typedef void (*thread_function)(void *);

/** A generic thread class.
 * 
 * @ingroup amanita */
class Thread {
private:
/*#ifdef USE_SDL
	SDL_Thread *thread;
	SDL_mutex *mutex;
	uint32_t timer;*/
#ifdef USE_PTHREADS
	pthread_t *thread;
	pthread_mutex_t *mutex;
	timeval time;
#endif
#ifdef USE_WIN32_THREADS
	HANDLE thread;
	HANDLE mutex;
	timeval time;
#endif
	thread_function function;
	void *data;

	void createMutex();
	void deleteMutex();

public:
	Thread();
	~Thread();

	thread_function getFunction() { return function; }
	void *getData() { return data; }

	/** Start thread
	 * @param f Function to be called by thread
	 * @param d Data to be sent through the function
	 * @param p Priority of thread, a value of 0-100, where 100 is max, with any other value no priority is set */
	void start(thread_function f,void *d=0,int p=-1);
	void stop();
	void kill();
	bool isRunning() { return thread!=0; }

	void lock();
	void unlock();

	void pauseFPS(int fps);
	void pause(int millis) {
/*#ifdef USE_SDL
		SDL_Delay(millis);*/
#ifdef USE_PTHREADS
		usleep(millis*1000);
#endif
#ifdef USE_WIN32_THREADS
		Sleep(millis);
#endif
	}
};

}; /* namespace a */


#endif /* _AMANITA_THREAD_H */
