#ifndef _pcb_h_
#define _pcb_h_
#include "listpcb.h"
#include "listsign.h"
#include "thread.h"
#include "SCHEDULE.H"

enum Status
{
	AWAKENED, SIGNALED, IDLE, BLOCKED, NEW, RUNNING, FINISHED, READY, NOT_SPECIFIED
};
enum Signals
{
	THREAD_KILLED_SIGNAL, CHILD_FINISHED_SIGNAL, THREAD_FINISHED_SIGNAL
};
enum GlobalSignalState
{
	UNBLOCKED_GLOBALLY, BLOCKED_GLOBALLY
};
enum LocalSignalState
{
	UNBLOCKED_LOCALLY, BLOCKED_LOCALLY
};

#define mainID -1
#define idleID -2

#define minStackSize 1024
#define maxStackSize 65535

#define maxTimeSlice 65535
#define idleTimeSlice 1

#define mainThreadSpecifier 0
#define idleThreadSpecifier 1

#define LOCK asm pushf; asm cli;
#define UNLOCK asm popf;

#define numOfSignals 16

class PCB
{
public:
	unsigned myTimeToWait;
	unsigned myStackSize;
	unsigned myTimeSlice;
	unsigned *stack;
	unsigned bp;
	unsigned sp;
	unsigned ss;

	static ListPCB *listOfAllPCB;
	static volatile PCB *running;
	static volatile PCB *idle;
	static volatile PCB *main;
	static void wrapper();
	static ID posID;

	Status returnState;
	Status status;

	ListPCB *blocked;

	ID mySemID;
	ID id;

	PCB(int specifier = 0);
	PCB(Thread *thread, StackSize stackSize, Time timeSlice);
	~PCB();

	Thread* getMyThread();

	void handleAllSignals();

	void signal(SignalId signal);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);

	void blockSignal(SignalId signal);
	void unblockSignal(SignalId signal);

	static GlobalSignalState globalState[numOfSignals];
	LocalSignalState localState[numOfSignals];

	ListSIGN *signalHandlers;
	ListSIGN *signals;

	static SignalHandler killThread;
	static void kill();

	int killFlag;

	PCB *parent;
private:
	Thread *myThread;
};

#endif
