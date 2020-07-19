#ifndef _ivt_h_
#define _ivt_h_
#include "kevent.h"

typedef void interrupt(*pInterrupt)(...);

#define PREPAREENTRY(IVTNo, callOld)\
		void interrupt intRoutine##IVTNo(...);\
		IVTEntry newEntry##IVTNo(IVTNo, intRoutine##IVTNo);\
		void interrupt intRoutine##IVTNo(...) {\
			 newEntry##IVTNo.signal();\
			 if (callOld)\
			 	 newEntry##IVTNo.callOldRoutine();\
			 dispatch();\
		}

class IVTEntry
{
public:
	static IVTEntry *IVT[256];
	Event *event;

	void signal();

	void callOldRoutine();
	void callNewRoutine();

	void initialize(IVTNo myIVTNo, pInterrupt oldRoutine, pInterrupt newRoutine);
	void restore(IVTNo myIVTNo, pInterrupt oldRoutine);

	IVTEntry(IVTNo ivtNo, pInterrupt newRout);
	~IVTEntry();
private:
	pInterrupt oldRoutine;
	pInterrupt newRoutine;
	IVTNo myIVTNo;
};

#endif
