
#include "_config.h"

//#ifdef USE_SDL
//#else
	#include <stdlib.h>
//#endif

#include <amanita/Thread.h>



namespace a {


/*#ifdef USE_SDL
static int Thread_run(void *d) {
	Thread &t = *(Thread *)d;
	thread_function f = t.getFunction();
	f(t.getData());
	return 0;
}*/
#ifdef USE_PTHREADS
static void *Thread_run(void *d) {
	Thread &t = *(Thread *)d;
	thread_function f = t.getFunction();
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
	f(t.getData());
	pthread_exit(0);
	return 0;
}
#endif
#ifdef USE_WIN32_THREADS
static DWORD WINAPI Thread_run(void *d) {
	Thread &t = *(Thread *)d;
	thread_function f = t.getFunction();
	f(t.getData());
	return 0;
}
#endif



Thread::Thread() : thread(0),mutex(0),function(0),data(0) {
}

Thread::~Thread() {
	if(thread) kill();
	if(mutex) deleteMutex();
}



void Thread::start(thread_function f,void *d,int p) {
	if(thread) kill();
	function = f;
	data = d;
/*#ifdef USE_SDL
	timer = SDL_GetTicks();
	thread = SDL_CreateThread(Thread_run,this);*/
#ifdef USE_PTHREADS
	gettimeofday(&time,0);
	thread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(thread,0,Thread_run,this);
	if(p>=0 && p<=100) {
		int policy,min,max;
		struct sched_param param;
		pthread_getschedparam(*thread,&policy,&param);
		min = sched_get_priority_max(policy);
		max = sched_get_priority_max(policy);
		param.sched_priority = min+((max-min)*p/100);
		pthread_setschedparam(*thread,policy,&param);
	}
#endif
#ifdef USE_WIN32_THREADS
	gettimeofday(&time,0);
	thread = CreateThread(NULL,0,Thread_run,this,0,NULL);
	if(p>=0 && p<=100)
		SetThreadPriority(thread,
			THREAD_PRIORITY_LOWEST+((THREAD_PRIORITY_HIGHEST-THREAD_PRIORITY_LOWEST)*p/100));
#endif
}

void Thread::stop() {
/*#ifdef USE_SDL
	if(thread) SDL_WaitThread(thread,0);*/
#ifdef USE_PTHREADS
	if(thread) {
		pthread_t *t = thread;
		thread = 0;
		pthread_join(*t,0);
		free(t);
	}
#endif
#ifdef USE_WIN32_THREADS
	if(thread) {
		HANDLE t = thread;
		thread = 0;
		WaitForSingleObject(t,INFINITE);
	}
#endif
	function = 0;
	data = 0;
}

void Thread::kill() {
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


void Thread::createMutex() {
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

void Thread::deleteMutex() {
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

void Thread::lock() {
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

void Thread::unlock() {
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

void Thread::pauseFPS(int fps) {
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
//printf("Thread::pauseFPS(time=%d,now=%d,delay=%d,usleep=%d)\n",
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

}; /* namespace a */



