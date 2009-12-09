
#include "../config.h"

#ifdef LIBAMANITA_SDL
#elif defined __linux__
	#include <stdlib.h>
	#include <signal.h>
#endif /* LIBAMANITA_SDL */

#include <libamanita/aThread.h>


aThread::aThread() : h(0) {
#ifdef LIBAMANITA_SDL
	t = SDL_GetTicks();
#elif defined __linux__
	t = clock();
#endif /* LIBAMANITA_SDL */
}

aThread::~aThread() {
	if(h) kill();
}

void aThread::start(thread_function f,void *d) {
	if(h) kill();
#ifdef LIBAMANITA_SDL
	h = SDL_CreateThread(f,d);
#elif defined __linux__
	h = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(h,0,f,d);
#endif /* LIBAMANITA_SDL */
}

void aThread::stop() {
#ifdef LIBAMANITA_SDL
	if(h) SDL_WaitThread(h,0);
	t = SDL_GetTicks();
#elif defined __linux__
	if(h) {
		pthread_join(*h,0);
		free(h);
	}
	t = clock();
#endif /* LIBAMANITA_SDL */
	h = 0;
}

void aThread::kill() {
#ifdef LIBAMANITA_SDL
	if(h) SDL_KillThread(h);
	t = SDL_GetTicks();
#elif defined __linux__
	if(h) {
		pthread_kill(*h,0);
		free(h);
	}
	t = clock();
#endif /* LIBAMANITA_SDL */
	h = 0;
}

void aThread::pauseFPS(int fps) {
#ifdef LIBAMANITA_SDL
	uint32_t ticks = SDL_GetTicks();
	int pause = t-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) SDL_Delay(pause);
	t = ticks+pause+(1000/fps);

#elif defined __linux__
	clock_t ticks = clock();
	int pause = t-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) usleep((pause*1000000)/CLOCKS_PER_SEC);
	t = ticks+pause+(1000000/fps);
#endif /* LIBAMANITA_SDL */
}
