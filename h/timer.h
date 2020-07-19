#ifndef _timer_h_
#define _timer_h_
#include <dos.h>

typedef void interrupt(*pInterrupt)(...);

void createAllThreads();
void initializeTimerRoutines();
void interrupt timer(...);
void restoreTimerRoutines();
void deleteAllThreads();
void updateSleepingLists();

#endif
