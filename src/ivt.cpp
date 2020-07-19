#include "ivt.h"
#include <dos.h>
#include "iostream.h"

IVTEntry *IVTEntry::IVT[256] = { 0 };

inline void IVTEntry::initialize(IVTNo myIVTNo, pInterrupt oldRoutine, pInterrupt newRoutine)
{
#ifndef BCC_BLOCK_IGNORE
	oldRoutine = getvect(myIVTNo);
	setvect(myIVTNo, newRoutine);
#endif
}

inline void IVTEntry::restore(IVTNo myIVTNo, pInterrupt oldRoutine)
{
#ifndef BCC_BLOCK_IGNORE
	setvect(myIVTNo, oldRoutine);
#endif
}

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRout) : myIVTNo(ivtNo), oldRoutine(0), event(0), newRoutine(newRout)
{
	LOCK
	if (!IVTEntry::IVT[myIVTNo])
	{
		IVTEntry::IVT[myIVTNo] = this;
	}
	initialize(myIVTNo, oldRoutine, newRoutine);
	UNLOCK
}

IVTEntry::~IVTEntry()
{
	LOCK
	restore(myIVTNo, oldRoutine);
	IVTEntry::IVT[myIVTNo] = 0;
	UNLOCK
}

void IVTEntry::signal()
{
	if (IVTEntry::IVT[myIVTNo]->event)
	{
		event->signal();
	}
}

void IVTEntry::callOldRoutine()
{
	oldRoutine();
}

void IVTEntry::callNewRoutine()
{
	newRoutine();
}
