#include "Thread.h"
#include <thread>


Thread::Thread(void* function, void* args)
{
	thread(function, args);
}


Thread::~Thread()
{
}
