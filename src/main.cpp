#include <iostream.h>
#include "pcb.h"
#include "timer.h"
#include "kersem.h"

extern int userMain(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	createAllThreads();
	initializeTimerRoutines();

	int userMainRetVal = userMain(argc, argv);

	restoreTimerRoutines();
	deleteAllThreads();

	return userMainRetVal;
}
