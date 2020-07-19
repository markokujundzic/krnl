#include "kersem.h"
#include "iostream.h"

ListSEM *KernelSem::listOfAllSemaphores = new ListSEM();
ListPCB *KernelSem::listOfAllSleepingPCB = 0;
ID KernelSem::semID = 0;

void KernelSem::block(Time maxTimeToWait)
{
	PCB::running->status = BLOCKED;
	blocked->push_back((PCB*) PCB::running);
	if (maxTimeToWait > 0)
	{
		PCB::running->myTimeToWait = maxTimeToWait;
		PCB::running->mySemID = id;
		KernelSem::listOfAllSleepingPCB->push_back((PCB*) PCB::running);
	}
}

void KernelSem::deblock()
{
	PCB *insertMe = blocked->pop_front();
	insertMe->status = READY;
	insertMe->returnState = SIGNALED;
	insertMe->myTimeToWait = 0;
	insertMe->mySemID = 0;
	KernelSem::listOfAllSleepingPCB->remove(insertMe);
	Scheduler::put(insertMe);
}

Semaphore* KernelSem::getMySemaphore()
{
	return mySemaphore;
}

KernelSem* KernelSem::getKernelSemById(ID id)
{
	ListSEM::Elem *curr = KernelSem::listOfAllSemaphores->head;
	while (curr)
	{
		if (id == curr->kernelSem->id)
		{
			return curr->kernelSem;
		}
		curr = curr->next;
	}
	return 0;
}

int KernelSem::signal(int n)
{
	LOCK
	if (n >= 0)
	{
		if (n == 0 && val++ < 0)
		{
			deblock();
			UNLOCK
			return 0;
		}
		else if (n > 0 && n > blocked->length)
		{
			// npr. signal 5, a ima 3 blokirana

			int retVal = blocked->length;

			while (blocked->length)
			{
				deblock();
				val++;
			}

			UNLOCK
			return retVal;
		}
		else if (n > 0 && n <= blocked->length)
		{
			// npr. signal 3, a ima 5 blokiranih

			int countdown = n;

			while (countdown)
			{
				deblock();
				val++;
				countdown--;
			}
			UNLOCK
			return n;
		}
	}
	UNLOCK
	return n;
}

int KernelSem::wait(Time maxTimeToWait)
{
	LOCK
	if (--val < 0)
	{
		block(maxTimeToWait);
		UNLOCK
		dispatch();
		return PCB::running->returnState;
	}
	else
	{
		UNLOCK
		return 1;
	}
}

KernelSem::KernelSem(Semaphore *semaphore, int init)
{
	LOCK
	mySemaphore = semaphore;
	blocked = new ListPCB();
	id = ++semID;
	val = init;

	KernelSem::listOfAllSemaphores->push_back(this);
	UNLOCK
}

KernelSem::~KernelSem()
{
	LOCK
	while (blocked->length)
	{
		PCB *insertMe = blocked->pop_front();
		insertMe->status = READY;
		insertMe->returnState = SIGNALED;
		insertMe->myTimeToWait = 0;
		insertMe->mySemID = 0;
		KernelSem::listOfAllSleepingPCB->remove(insertMe);
		Scheduler::put(insertMe);
	}
	KernelSem::listOfAllSemaphores->remove(this);

	delete blocked;
	UNLOCK
}

