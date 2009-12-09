#ifndef _LIBAMANITA_SDL_THREAD_H
#define _LIBAMANITA_SDL_THREAD_H

/**
 * @file libamanita/aThread.h
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>

#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_thread.h>
#elif defined __linux__
	#include <pthread.h>
	#include <unistd.h>
	#include <time.h>
#endif /* LIBAMANITA_SDL */


/** Callback function used by aSocket to handle events.
 *
 * This function is one of the most important in this interface, because it makes it possible
 * for the program to intercept all communication over the network. It also sends error messages
 * and warnings.
 */
#ifdef LIBAMANITA_SDL
typedef int (*thread_function)(void *);
#elif defined __linux__
typedef void *(*thread_function)(void *);
#endif /* LIBAMANITA_SDL */


class aThread {
private:
#ifdef LIBAMANITA_SDL
	uint32_t t;
	SDL_Thread *h;
#elif defined __linux__
	clock_t t;
	pthread_t *h;
#endif /* LIBAMANITA_SDL */

public:
	aThread();
	~aThread();

	void start(thread_function f,void *d=0);
	void stop();
	void kill();
	bool isRunning() { return h!=0; }

	void pauseFPS(int fps);
	void pause(int millis) {
#ifdef LIBAMANITA_SDL
		SDL_Delay(millis);
#elif defined __linux__
		usleep(millis*1000);
#endif /* LIBAMANITA_SDL */
	}
};

#endif /* _LIBAMANITA_SDL_THREAD_H */
