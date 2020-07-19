#include "listsem.h"
#include "kersem.h"

ListSEM::ListSEM() : head(0), tail(0), length(0)
{

}

ListSEM::~ListSEM()
{
	while (length)
	{
		length--;
		Elem *temp = head;
		head = head->next;
		delete temp;
	}
	head = tail = 0;
	length = 0;
}

void ListSEM::push_front(KernelSem *insertMe)
{
	Elem *temp = new Elem(insertMe);
	if (!length)
	{
		head = tail = temp;
	}
	else
	{
		temp->next = head;
		head = temp;
	}
	length++;
}

void ListSEM::push_back(KernelSem *insertMe)
{
	Elem *temp = new Elem(insertMe);
	if (!length)
	{
		head = tail = temp;
	}
	else
	{
		tail = tail->next = temp;
	}
	length++;
}

KernelSem* ListSEM::pop_front()
{
	if (length)
	{
		length--;
		Elem *temp = head;
		head = head->next;
		KernelSem *retKernelSem = temp->kernelSem;
		if (!length)
		{
			head = tail = 0;
		}
		delete temp;
		return retKernelSem;
	}
	else
		return 0;
}

KernelSem* ListSEM::pop_back()
{
	if (length)
	{
		length--;
		Elem *temp = tail;
		KernelSem *retKernelSem = temp->kernelSem;
		if (!length)
		{
			head = tail = 0;
		}
		else
		{
			Elem *prev = 0;
			Elem *curr = head;
			while (curr->next)
			{
				prev = curr;
				curr = curr->next;
			}
			tail = prev;
		}
		delete temp;
		return retKernelSem;
	}
	else
		return 0;
}

void ListSEM::remove(KernelSem *removeMe)
{
	Elem *prev = 0;
	Elem *curr = head;
	while (curr)
	{
		if (curr->kernelSem == removeMe)
		{
			length--;
			if (!prev)
			{
				head = curr->next;
			}
			else
			{
				prev->next = curr->next;
			}
			if (curr == tail)
			{
				tail = prev;
			}
			delete curr;
			break;
		}
		prev = curr;
		curr = curr->next;
	}
}

KernelSem* ListSEM::getById(ID id)
{
	Elem *curr = head;
	while (curr)
	{
		if (id == curr->kernelSem->id)
		{
			return curr->kernelSem;
		}
		curr = curr->next;
	}
	return 0;
}

