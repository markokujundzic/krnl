#ifndef _kevent_h_
#define _kevent_h_
#include "event.h"
#include "pcb.h"
#include "ivt.h"

class KernelEv
{
public:
	KernelEv(IVTNo ivtNo, Event *eventToLink, PCB *pcbToLink);
	~KernelEv();

	void signal();
	void wait();

	Event *myEvent;
	PCB *myPCB;

	int val;
private:
	IVTNo myIVTNo;
};

#endif
