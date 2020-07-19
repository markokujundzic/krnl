#include "timer.h"
#include "pcb.h"
#include "kersem.h"
#include "iostream.h"

volatile int requested_context_switch = 0;
volatile int counter = defaultTimeSlice;

pInterrupt oldTimerRoutine;

unsigned tsp;
unsigned tss;
unsigned tbp;

extern PCB* createIdleThread();
extern PCB* createMainThread();

extern void tick();

void process(ListPCB::Elem *curr)
{
	KernelSem::listOfAllSleepingPCB->length--;
	curr->pcb->returnState = AWAKENED;
	curr->pcb->status = READY;
	KernelSem::getKernelSemById(curr->pcb->mySemID)->val++;
	KernelSem::getKernelSemById(curr->pcb->mySemID)->blocked->remove(curr->pcb);
	Scheduler::put(curr->pcb);
}

void updateSleepingLists()
{
	int deleteFlag = 0;

	ListPCB::Elem *prev = 0;
	ListPCB::Elem *curr = KernelSem::listOfAllSleepingPCB->head;

	while (curr)
	{
		deleteFlag = 0;
		if (--curr->pcb->myTimeToWait == 0)
		{
			ListPCB::Elem *toDelete = curr;

			process(curr);

			if (!prev)
			{
				KernelSem::listOfAllSleepingPCB->head = curr->next;
			}
			else
			{
				prev->next = curr->next;
			}

			if (curr == KernelSem::listOfAllSleepingPCB->tail)
			{
				KernelSem::listOfAllSleepingPCB->tail = prev;
			}

			curr = curr->next;

			deleteFlag = 1;

			delete toDelete;
		}
		if (!deleteFlag)
		{
			prev = curr;
			curr = curr->next;
		}
	}
}

void createAllThreads()
{
	PCB::killThread = PCB::kill;
	PCB::idle = createIdleThread();
	PCB::main = createMainThread();
	PCB::running = PCB::main;
	KernelSem::listOfAllSleepingPCB = new ListPCB();
}

void deleteAllThreads()
{
	delete PCB::idle;
	delete PCB::main;
	delete PCB::listOfAllPCB;
	delete KernelSem::listOfAllSemaphores;
	delete KernelSem::listOfAllSleepingPCB;
}

void initializeTimerRoutines()
{
	LOCK
#ifndef BCC_BLOCK_IGNORE
	oldTimerRoutine = getvect(0x8);
	setvect(0x8, timer);
	setvect(0x60, oldTimerRoutine);
#endif
	UNLOCK
}

void restoreTimerRoutines()
{
	LOCK
#ifndef BCC_BLOCK_IGNORE
	setvect(0x8, oldTimerRoutine);
#endif
	UNLOCK
}

void interrupt timer(...)
{
	int idleFlag = 0;

	if (!requested_context_switch)
	{
		tick();
		updateSleepingLists();
		counter--;
	}
	if (!counter || requested_context_switch)
	{
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
#endif
		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		if (PCB::running->status == RUNNING)
		{
			PCB::running->status = READY;
			Scheduler::put((PCB*) PCB::running);
		}

		PCB::running = Scheduler::get();

		if (!PCB::running)
		{
			PCB::running = PCB::idle;
			idleFlag = 1;
		}
		else
		{
			PCB::running->status = RUNNING;
		}

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		counter = PCB::running->myTimeSlice;
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
#endif
	}
	if (!idleFlag)
	{
		((PCB*) PCB::running)->handleAllSignals();
	}
#ifndef BCC_BLOCK_IGNORE
	if (!requested_context_switch)
	{
		asm int 60h;
	}
#endif
	requested_context_switch = 0;
}
