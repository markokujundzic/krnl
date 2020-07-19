#ifndef _kersem_h_
#define _kersem_h_
#include "listsem.h"
#include "semaphor.h"
#include "pcb.h"

class KernelSem
{
public:
	static KernelSem* getKernelSemById(ID id);
	static ListSEM *listOfAllSemaphores;
	static ListPCB *listOfAllSleepingPCB;
	static ID semID;

	int val;

	ListPCB *blocked;
	ID id;

	int wait(Time maxTimeToWait);
	int signal(int n);

	void block(Time maxTimeToWait);
	void deblock();

	KernelSem(Semaphore *semaphore, int init);
	~KernelSem();

	Semaphore* getMySemaphore();
private:
	Semaphore *mySemaphore;
};

#endif
