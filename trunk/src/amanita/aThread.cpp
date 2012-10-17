
#include "_config.h"

//#ifdef USE_SDL
//#else
	#include <stdlib.h>
//#endif

#include <amanita/aThread.h>



/*#ifdef USE_SDL
int aThread::_run(void *d) {
	aThread &t = *(aThread *)d;
	t.function(t.data);
	return 0;
}*/
#ifdef USE_PTHREADS
void *aThread::_run(void *d) {
	aThread &t = *(aThread *)d;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
	t.function(t.data);
	pthread_exit(0);
	return 0;
}
#endif
#ifdef USE_WIN32_THREADS
DWORD WINAPI aThread::_run(void *d) {
	aThread &t = *(aThread *)d;
	t.function(t.data);
	return 0;
}
#endif




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
/*#ifdef USE_SDL
	timer = SDL_GetTicks();
	thread = SDL_CreateThread(_run,this);*/
#ifdef USE_PTHREADS
	gettimeofday(&time,0);
	thread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(thread,0,_run,this);
#endif
#ifdef USE_WIN32_THREADS
	gettimeofday(&time,0);
	CreateThread(NULL,0,_run,this,0,NULL);
#endif
}

void aThread::stop() {
/*#ifdef USE_SDL
	if(thread) SDL_WaitThread(thread,0);*/
#ifdef USE_PTHREADS
	if(thread) {
		pthread_join(*thread,0);
		free(thread);
	}
#endif
#ifdef USE_WIN32_THREADS
	if(thread) WaitForSingleObject(thread,INFINITE);
#endif
	thread = 0;
	function = 0;
	data = 0;
}

void aThread::kill() {
/*#ifdef USE_SDL
	if(thread) SDL_KillThread(thread);*/
#ifdef USE_PTHREADS
	if(thread) {
		pthread_cancel(*thread);
		free(thread);
	}
#endif
#ifdef USE_WIN32_THREADS
	if(thread) TerminateThread(thread,0);
#endif
	thread = 0;
	function = 0;
	data = 0;
}


void aThread::createMutex() {
	if(mutex) deleteMutex();
/*#ifdef USE_SDL
	mutex = SDL_CreateMutex();*/
#ifdef USE_PTHREADS
	mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,0);
#endif
#ifdef USE_WIN32_THREADS
	mutex = CreateMutex(0,false,0);
#endif
}

void aThread::deleteMutex() {
	if(!mutex) return;
/*#ifdef USE_SDL
	SDL_DestroyMutex(mutex);*/
#ifdef USE_PTHREADS
	pthread_mutex_destroy(mutex);
	free(mutex);
#endif
#ifdef USE_WIN32_THREADS
	CloseHandle(mutex);
#endif
	mutex = 0;
}

void aThread::lock() {
	if(!mutex) createMutex();
/*#ifdef USE_SDL
	SDL_mutexP(mutex);*/
#ifdef USE_PTHREADS
	pthread_mutex_lock(mutex);
#endif
#ifdef USE_WIN32_THREADS
	WaitForSingleObject(mutex,INFINITE);
#endif
}

void aThread::unlock() {
	if(!mutex) return;
/*#ifdef USE_SDL
	SDL_mutexV(mutex);*/
#ifdef USE_PTHREADS
	pthread_mutex_unlock(mutex);
#endif
#ifdef USE_WIN32_THREADS
	ReleaseMutex(mutex);
#endif
}

//#include <stdio.h>

void aThread::pauseFPS(int fps) {
/*#ifdef USE_SDL
	uint32_t ticks = SDL_GetTicks();
	int pause = timer-ticks;
//fprintf(out,"ticks=%d,next=%d,pause=%d, => %d\n",ticks,next,pause,ticks+pause+(1000/fps));
	if(pause>0) SDL_Delay(pause);
	timer = ticks+pause+(1000/fps);*/
	timeval now;
	gettimeofday(&now,0);
	fps = 1000000/fps;
	int delay = (now.tv_sec*1000000+now.tv_usec)-(time.tv_sec*1000000+time.tv_usec);
	int delay2 = 2*fps-delay;
	if(delay2>fps) delay2 = fps;
//printf("aThread::pauseFPS(time=%d,now=%d,delay=%d,usleep=%d)\n",
//		(int)(time.tv_sec*1000000+time.tv_usec),(int)(now.tv_sec*1000000+now.tv_usec),delay,delay2);
#ifdef USE_PTHREADS
	if(delay2>0) usleep(delay2);
#endif
#ifdef USE_WIN32_THREADS
	if(delay2>0) Sleep(delay2/1000);
#endif
	time = now;
//#endif
}



