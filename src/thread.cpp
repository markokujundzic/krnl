#include "pcb.h"
#include <iostream.h>

extern void interrupt timer(...);
extern int requested_context_switch;

void Thread::signal(SignalId signal)
{
	myPCB->signal(signal);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
	myPCB->swap(id, hand1, hand2);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler)
{
	myPCB->registerHandler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalId id)
{
	myPCB->unregisterAllHandlers(id);
}

void Thread::blockSignal(SignalId signal)
{
	myPCB->blockSignal(signal);
}

void Thread::unblockSignal(SignalId signal)
{
	myPCB->unblockSignal(signal);
}

void Thread::blockSignalGlobally(SignalId signal)
{
	LOCK
	if (signal >= 0 && signal < numOfSignals)
	{
		PCB::globalState[signal] = BLOCKED_GLOBALLY;
	}
	UNLOCK
}

void Thread::unblockSignalGlobally(SignalId signal)
{
	LOCK
	if (signal >= 0 && signal < numOfSignals)
	{
		PCB::globalState[signal] = UNBLOCKED_GLOBALLY;
	}
	UNLOCK
}

ID Thread::getID()
{
	return myPCB->id;
}

ID Thread::getRunningId()
{
	return PCB::running->id;
}

Thread* Thread::getThreadById(ID id)
{
	if (id != mainID && id != idleID)
	{
		ListPCB::Elem *curr = PCB::listOfAllPCB->head;

		while (curr)
		{
			if (id == curr->pcb->id)
			{
				return curr->pcb->getMyThread();
			}
			curr = curr->next;
		}
	}
	return 0;
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	myPCB = new PCB(this, stackSize, timeSlice);
}

void Thread::start()
{
	LOCK
	if (myPCB->status == NEW)
	{
		myPCB->status = READY;
		Scheduler::put(myPCB);
	}
	UNLOCK
}

void Thread::waitToComplete()
{
	if (myPCB != PCB::running && myPCB->status != FINISHED)
	{
		LOCK
		PCB::running->status = BLOCKED;
		myPCB->blocked->push_back((PCB*) PCB::running);
		UNLOCK
		dispatch();
	}
}

Thread::~Thread()
{
	delete myPCB;
}

void dispatch()
{
	LOCK
	requested_context_switch = 1;
	timer();
	UNLOCK
}
