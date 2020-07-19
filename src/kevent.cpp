#include "kevent.h"
#include <iostream.h>

KernelEv::KernelEv(IVTNo ivtNo, Event *eventToLink, PCB *pcbToLink) : myIVTNo(ivtNo), myEvent(eventToLink), myPCB(pcbToLink), val(0)
{
	LOCK
	if (!IVTEntry::IVT[myIVTNo]->event)
	{
		IVTEntry::IVT[myIVTNo]->event = myEvent;
	}
	UNLOCK
}

KernelEv::~KernelEv()
{
	LOCK
	if (IVTEntry::IVT[myIVTNo]->event)
	{
		IVTEntry::IVT[myIVTNo]->event = 0;
	}
	UNLOCK
}

void KernelEv::wait()
{
	LOCK
	if (myPCB == PCB::running && --val < 0)
	{
		myPCB->status = BLOCKED;
		UNLOCK
		dispatch();
	}
	else
	{
		UNLOCK
	}
}

void KernelEv::signal()
{
	LOCK
	if (val++ < 0)
	{
		myPCB->status = READY;
		Scheduler::put(myPCB);
	}
	else
	{
		val = 1;
	}
	UNLOCK
}
