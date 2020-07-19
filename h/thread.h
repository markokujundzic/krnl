#ifndef _thread_h_
#define _thread_h_
#include "listpcb.h"

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class PCB;

class Thread
{
public:
	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getID();
	static ID getRunningId();
	static Thread* getThreadById(ID id);

	void signal(SignalId signal);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);

	void blockSignal(SignalId signal);
	void unblockSignal(SignalId signal);

	static void blockSignalGlobally(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
protected:
	friend class PCB;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run()
	{

	}
private:
	PCB *myPCB;
};

void dispatch();

#endif
