#ifndef _semlist_h_
#define _semlist_h_
#include "thread.h"

class KernelSem;

class ListSEM
{
public:
	struct Elem
	{
		KernelSem *kernelSem;
		Elem *next;
		Elem(KernelSem *kernelSem) : kernelSem(kernelSem), next(0)
		{
		}
	};

	Elem *head, *tail;
	int length;

	ListSEM();
	~ListSEM();

	void remove(KernelSem *removeMe);

	void push_front(KernelSem *insertMe);
	void push_back(KernelSem *insertMe);

	KernelSem* pop_front();
	KernelSem* pop_back();

	KernelSem* getById(ID id);
};

#endif
