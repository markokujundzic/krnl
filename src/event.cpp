#include "event.h"
#include "pcb.h"
#include "kevent.h"

Event::Event(IVTNo ivtNo)
{
	myImpl = new KernelEv(ivtNo, this, (PCB*) PCB::running);
}

Event::~Event()
{
	delete myImpl;
}

void Event::wait()
{
	myImpl->wait();
}

void Event::signal()
{
	myImpl->signal();
}
