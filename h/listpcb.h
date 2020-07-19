#ifndef _list_h_
#define _list_h_

class PCB;
typedef int ID;

class ListPCB
{
public:
	struct Elem
	{
		PCB *pcb;
		Elem *next;
		Elem(PCB *pcb) : pcb(pcb), next(0)
		{
		}
	};

	Elem *head, *tail;
	int length;

	ListPCB();
	~ListPCB();

	void remove(PCB *removeMe);

	void push_front(PCB *insertMe);
	void push_back(PCB *insertMe);

	PCB* pop_front();
	PCB* pop_back();

	PCB* getById(ID id);
};

#endif
