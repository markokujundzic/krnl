#include "semaphor.h"
#include "kersem.h"
#include "pcb.h"

Semaphore::Semaphore(int init)
{
	myImpl = new KernelSem(this, init);
}

Semaphore::~Semaphore()
{
	delete myImpl;
}

int Semaphore::wait(Time maxTimeToWait)
{
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n)
{
	return myImpl->signal(n);
}

int Semaphore::val() const
{
	return myImpl->val;
}
