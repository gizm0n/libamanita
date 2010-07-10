
#include "config.h"

#ifdef LIBAMANITA_SDL
#elif defined __linux__
	#include <stdlib.h>
	#include <sys/time.h>
#endif /* LIBAMANITA_SDL */

#include <libamanita/aThread.h>



#ifdef LIBAMANITA_SDL
int aThread::_run(void *d) {
	aThread &t = *(aThread *)d;
	t.function(t.data);
	return 0;
}
#elif defined __linux__
void *aThread::_run(void *d) {
	aThread &t = *(aThread *)d;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
	t.function(t.data);
	pthread_exit(0);
	return 0;
}
#endif /* LIBAMANITA_SDL */




aThread::aThread() : thread(0),mutex(0),function(0),data(0) {
}

aThread::~aThread() {
	if(thread) kill();
	if(mutex) deleteMutex();
}



void aThread::start(thread_function f,void *d) {
	if(thread) kill();
	function = f;
	data = d;
#ifdef LIBAMANITA_SDL
	timer = SDL_GetTicks();
	thread = SDL_CreateThread(_run,this);
#elif defined __linux__
	gettimeofday(&time,0);
	thread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(thread,0,_run,this);
#endif /* LIBAMANITA_SDL */
}

void aThread::stop() {
#ifdef LIBAMANITA_SDL
	if(thread) SDL_WaitThread(thread,0);
#elif defined __linux__
	if(thread) {
		pthread_join(*thread,0);
		free(thread);
	}
#endif /* LIBAMANITA_SDL */
	thread = 0;
	function = 0;
	data = 0;
}

void aThread::kill() {
#ifdef LIBAMANITA_SDL
	if(thread) SDL_KillThread(thread);
#elif defined __linux__
	if(thread) {
		pthread_cancel(*thread);
		free(thread);
	}
#endif /* LIBAMANITA_SDL */
	thread = 0;
	function = 0;
	data = 0;
}


void aThread::createMutex() {
	if(mutex) deleteMutex();
#ifdef LIBAMANITA_SDL
	mutex = SDL_CreateMutex();
#elif defined __linux__
	mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,0);
#endif /* LIBAMANITA_SDL */
}

void aThread::deleteMutex() {
	if(!mutex) return;
#ifdef LIBAMANITA_SDL
	SDL_DestroyMutex(mutex);
	mutex = 0;
#elif defined __linux__
	pthread_mutex_destroy(mutex);
	free(mutex);
	mutex = 0;
#endif /* LIBAMANITA_SDL */
}

void aThread::lock() {
	if(!mutex) createMutex();
	int n;
#ifdef LIBAMANITA_SDL
	n =  SDL_mutexP(mutex);
#elif defined __linux__
	n =  pthread_mutex_lock(mutex);
#endif /* LIBAMANITA_SDL */
}

void aThread::unlock() {
	if(!mutex) return;
	int n;
#ifdef LIBAMANITA_SDL
	n = SDL_mutexV(mutex);
#elif defined __linux__
	n = pthread_mutex_unlock(mutex);
#endif /* LIBAMANITA_SDL */
}

#include <stdio.h>

void aThread::pauseFPS(int fps) {
#ifdef LIBAMANITA_SDL
	uint32_t ticks = SDL_GetTicks();
	int pause = timer-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) SDL_Delay(pause);
	timer = ticks+pause+(1000/fps);

#elif defined __linux__
	timeval now;
	gettimeofday(&now,0);
	fps = 1000000/fps;
	int delay = (now.tv_sec*1000000+now.tv_usec)-(time.tv_sec*1000000+time.tv_usec);
	int delay2 = 2*fps-delay;
	if(delay2>fps) delay2 = fps;
//printf("aThread::pauseFPS(time=%d,now=%d,delay=%d,usleep=%d)\n",
//		(int)(time.tv_sec*1000000+time.tv_usec),(int)(now.tv_sec*1000000+now.tv_usec),delay,delay2);
	if(delay2>0) usleep(delay2);
	time = now;
#endif /* LIBAMANITA_SDL */
}



