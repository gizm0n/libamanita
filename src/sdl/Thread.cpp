
#include "../config.h"
#include <libamanita/sdl/Thread.h>


Thread::Thread() : t(0),h(0) {
}

Thread::~Thread() {
	if(h) kill();
}

void Thread::start(int (*f)(void *data),void *d) {
	if(h) kill();
	h = SDL_CreateThread(f,d);
}

void Thread::stop() {
	if(h) SDL_WaitThread(h,0);
	t = 0,h = 0;
}

void Thread::kill() {
	if(h) SDL_KillThread(h);
	t = 0,h = 0;
}

void Thread::pauseFPS(int fps) {
	Uint32 ticks = SDL_GetTicks();
	int pause = t-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) SDL_Delay(pause);
	t = ticks+pause+(1000/fps);
}
