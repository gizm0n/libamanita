
#include "../config.h"
#include <libamanita/sdl/aThread.h>


aThread::aThread() : t(0),h(0) {
}

aThread::~aThread() {
	if(h) kill();
}

void aThread::start(int (*f)(void *data),void *d) {
	if(h) kill();
	h = SDL_CreateThread(f,d);
}

void aThread::stop() {
	if(h) SDL_WaitThread(h,0);
	t = 0,h = 0;
}

void aThread::kill() {
	if(h) SDL_KillThread(h);
	t = 0,h = 0;
}

void aThread::pauseFPS(int fps) {
	Uint32 ticks = SDL_GetTicks();
	int pause = t-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) SDL_Delay(pause);
	t = ticks+pause+(1000/fps);
}
