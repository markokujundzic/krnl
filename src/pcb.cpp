#include "pcb.h"
#include <dos.h>
#include <iostream.h>

GlobalSignalState PCB::globalState[numOfSignals] = { UNBLOCKED_GLOBALLY };

ListPCB *PCB::listOfAllPCB = new ListPCB();

SignalHandler PCB::killThread = 0;

volatile PCB *PCB::running = 0;

volatile PCB *PCB::idle = 0;

volatile PCB *PCB::main = 0;

ID PCB::posID = 0;

void PCB::kill()
{
	LOCK
	while (PCB::running->blocked->length)
	{
		PCB *temp = PCB::running->blocked->pop_front();
		temp->status = READY;
		Scheduler::put(temp);
	}

	PCB::running->status = FINISHED;

	((PCB*) PCB::running)->signal(THREAD_FINISHED_SIGNAL);

	if (PCB::running->parent)
	{
		((PCB*) PCB::running)->parent->signal(CHILD_FINISHED_SIGNAL);
	}

	delete[] PCB::running->stack;
	delete PCB::running->blocked;
	delete PCB::running->signals;
	delete PCB::running->signalHandlers;

	PCB::running->stack = 0;
	PCB::running->blocked = 0;
	PCB::running->signals = 0;
	PCB::running->signalHandlers = 0;

	UNLOCK

	dispatch();
}

void PCB::handleAllSignals()
{
	SignalId tempId = 0;

	while (signals->length)
	{
		tempId = signals->pop_front();

		if (PCB::globalState[tempId] != BLOCKED_GLOBALLY && localState[tempId] != BLOCKED_LOCALLY)
		{
			if (tempId == THREAD_KILLED_SIGNAL)
			{
				killFlag++;
			}

			signalHandlers->callSelectedHandlers(tempId);
		}
	}
}

void PCB::signal(SignalId signal)
{
	if (signal >= 0 && signal < numOfSignals)
	{
		if (status == FINISHED && signal == THREAD_FINISHED_SIGNAL && signalHandlers->signalPresentCounter[THREAD_FINISHED_SIGNAL])
		{
			signalHandlers->callSelectedHandlers(THREAD_FINISHED_SIGNAL);
		}
		else if (this != PCB::running && signal != THREAD_FINISHED_SIGNAL)
		{
			signals->push_back(signal);
		}
	}
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
	signalHandlers->swap(id, hand1, hand2);
}

void PCB::registerHandler(SignalId signal, SignalHandler handler)
{
	signalHandlers->registerHandler(signal, handler);
}

void PCB::unregisterAllHandlers(SignalId id)
{
	signalHandlers->unregisterAllHandlers(id);
}

void PCB::blockSignal(SignalId signal)
{
	if (signal >= 0 && signal < numOfSignals)
	{
		localState[signal] = BLOCKED_LOCALLY;
	}
}

void PCB::unblockSignal(SignalId signal)
{
	if (signal >= 0 && signal < numOfSignals)
	{
		localState[signal] = UNBLOCKED_LOCALLY;
	}
}

void idleFunction()
{
	while (1);
}

PCB* createIdleThread()
{
	return new PCB(idleThreadSpecifier);
}

PCB* createMainThread()
{
	return new PCB(mainThreadSpecifier);
}

Thread* PCB::getMyThread()
{
	return myThread;
}

void initSignals(PCB *pcb, PCB *parent = 0)
{
	pcb->killFlag = 0;

	for (int i = 0; i < numOfSignals; i++)
	{
		pcb->localState[i] = UNBLOCKED_LOCALLY;
	}

	pcb->parent = parent;

	if (!parent)
	{
		pcb->signalHandlers->registerHandler(THREAD_KILLED_SIGNAL, PCB::killThread);
	}
	else
	{
		ListSIGN::Elem *curr = parent->signals->head;

		while (curr)
		{
			pcb->signals->push_back(curr->signalId);
			curr = curr->next;
		}

		curr = parent->signalHandlers->head;

		while (curr)
		{
			pcb->signalHandlers->push_back(curr->signalId, curr->signalHandler);
			curr = curr->next;
		}

		for (int i = 0; i < numOfSignals; i++)
		{
			pcb->localState[i] = parent->localState[i];
			pcb->signalHandlers->signalPresentCounter[i] = parent->signalHandlers->signalPresentCounter[i];
		}
	}
}

PCB::PCB(int specifier)
{
	LOCK
	if (specifier)
	{
		myTimeSlice = idleTimeSlice;
		myStackSize = defaultStackSize;
		blocked = new ListPCB();
		status = IDLE;
		myThread = 0;
		id = idleID;

		myTimeToWait = 0;
		mySemID = 0;
		returnState = NOT_SPECIFIED;

		ss = sp = bp = 0;

		myStackSize /= sizeof(unsigned);
		stack = new unsigned[myStackSize];

		signalHandlers = new ListSIGN();
		signals = new ListSIGN();
		initSignals(this);

#ifndef BCC_BLOCK_IGNORE
		stack[myStackSize - 1] = 0x200;
		stack[myStackSize - 2] = FP_SEG(idleFunction);
		stack[myStackSize - 3] = FP_OFF(idleFunction);
		sp = FP_OFF(stack + myStackSize - 12);
		ss = FP_SEG(stack + myStackSize - 12);
		bp = sp;
#endif
		PCB::listOfAllPCB->push_back(this);
	}
	else
	{
		myTimeSlice = maxTimeSlice;
		myStackSize = defaultStackSize;
		blocked = new ListPCB();
		status = RUNNING;
		myThread = 0;
		id = mainID;
		stack = 0;

		myTimeToWait = 0;
		mySemID = 0;
		returnState = NOT_SPECIFIED;

		ss = sp = bp = 0;

		signalHandlers = new ListSIGN();
		signals = new ListSIGN();
		initSignals(this);

		PCB::listOfAllPCB->push_back(this);
	}
	UNLOCK
}

PCB::PCB(Thread *thread, StackSize stackSize, Time timeSlice)
{
	LOCK
	myTimeSlice = timeSlice;
	myStackSize = stackSize;
	blocked = new ListPCB();
	status = NEW;
	myThread = thread;
	id = ++posID;

	myTimeToWait = 0;
	mySemID = 0;
	returnState = NOT_SPECIFIED;

	ss = sp = bp = 0;

	if (!timeSlice)
	{
		myTimeSlice = 65535;
	}

	if (!myStackSize)
	{
		myStackSize = defaultStackSize;
	}

	if (myStackSize < minStackSize)
	{
		myStackSize = minStackSize;
	}

	if (myStackSize > maxStackSize)
	{
		myStackSize = maxStackSize;
	}

	myStackSize /= sizeof(unsigned);
	stack = new unsigned[myStackSize];

	signalHandlers = new ListSIGN();
	signals = new ListSIGN();
	initSignals(this, (PCB*) PCB::running);

#ifndef BCC_BLOCK_IGNORE
	stack[myStackSize - 1] = 0x200;
	stack[myStackSize - 2] = FP_SEG(PCB::wrapper);
	stack[myStackSize - 3] = FP_OFF(PCB::wrapper);
	sp = FP_OFF(stack + myStackSize - 12);
	ss = FP_SEG(stack + myStackSize - 12);
	bp = sp;
#endif

	PCB::listOfAllPCB->push_back(this);
	UNLOCK
}

PCB::~PCB()
{
	LOCK
	PCB::listOfAllPCB->remove(this);

	if (!killFlag)
	{
		delete signalHandlers;
		delete signals;
		delete blocked;
		delete[] stack;
	}
	UNLOCK
}

void PCB::wrapper()
{
	PCB::running->myThread->run();

	LOCK
	while (PCB::running->blocked->length)
	{
		PCB *temp = PCB::running->blocked->pop_front();
		temp->status = READY;
		Scheduler::put(temp);
	}

	PCB::running->status = FINISHED;

	((PCB*) PCB::running)->signal(THREAD_FINISHED_SIGNAL);

	if (PCB::running->parent)
	{
		((PCB*) PCB::running)->parent->signal(CHILD_FINISHED_SIGNAL);
	}

	UNLOCK

	dispatch();
}
