#ifndef _LIBAMANITA_SDL_THREAD_H
#define _LIBAMANITA_SDL_THREAD_H

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

class Thread {
private:
	Uint32 t;
	SDL_Thread *h;

public:
	Thread();
	~Thread();

	void start(int (*f)(void *data),void *d=0);
	void stop();
	void kill();
	bool isRunning() { return h!=0; }

	void pauseFPS(int fps);
	void pause(int millis) { SDL_Delay(millis); }
};

#endif /* _LIBAMANITA_SDL_THREAD_H */
