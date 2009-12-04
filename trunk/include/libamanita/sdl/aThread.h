#ifndef _LIBAMANITA_SDL_THREAD_H
#define _LIBAMANITA_SDL_THREAD_H

/**
 * @file libamanita/sdl/aThread.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

class aThread {
private:
	uint32_t t;
	SDL_Thread *h;

public:
	aThread();
	~aThread();

	void start(int (*f)(void *data),void *d=0);
	void stop();
	void kill();
	bool isRunning() { return h!=0; }

	void pauseFPS(int fps);
	void pause(int millis) { SDL_Delay(millis); }
};

#endif /* _LIBAMANITA_SDL_THREAD_H */
