#include "listpcb.h"
#include "pcb.h"

ListPCB::ListPCB() : head(0), tail(0), length(0)
{

}

ListPCB::~ListPCB()
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

void ListPCB::push_front(PCB *insertMe)
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

void ListPCB::push_back(PCB *insertMe)
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

PCB* ListPCB::pop_front()
{
	if (length)
	{
		length--;
		Elem *temp = head;
		head = head->next;
		PCB *retPCB = temp->pcb;
		if (!length)
		{
			head = tail = 0;
		}
		delete temp;
		return retPCB;
	}
	else
		return 0;
}

PCB* ListPCB::pop_back()
{
	if (length)
	{
		length--;
		Elem *temp = tail;
		PCB *retPCB = temp->pcb;
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
		return retPCB;
	}
	else
		return 0;
}

void ListPCB::remove(PCB *removeMe)
{
	Elem *prev = 0;
	Elem *curr = head;
	while (curr)
	{
		if (curr->pcb->id == removeMe->id)
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

PCB* ListPCB::getById(ID id)
{
	Elem *curr = head;
	while (curr)
	{
		if (id == curr->pcb->id)
		{
			return curr->pcb;
		}
		curr = curr->next;
	}
	return 0;
}
