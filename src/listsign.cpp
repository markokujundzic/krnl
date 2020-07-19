#include "listsign.h"
#include <iostream.h>

ListSIGN::ListSIGN() : head(0), tail(0), length(0)
{
	for (int i = 0; i < numOfSignals; i++)
	{
		signalPresentCounter[i] = 0;
	}
}

ListSIGN::~ListSIGN()
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

void ListSIGN::push_front(SignalId id, SignalHandler handler)
{
	Elem *temp = new Elem(id, handler);
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

void ListSIGN::push_back(SignalId id, SignalHandler handler)
{
	Elem *temp = new Elem(id, handler);
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

SignalId ListSIGN::pop_front()
{
	if (length)
	{
		length--;
		Elem *temp = head;
		head = head->next;
		SignalId retId = temp->signalId;
		if (!length)
		{
			head = tail = 0;
		}
		delete temp;
		return retId;
	}
	else
		return 0;
}

SignalId ListSIGN::pop_back()
{
	if (length)
	{
		length--;
		Elem *temp = tail;
		SignalId retId = temp->signalId;
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
		return retId;
	}
	else
		return 0;
}

void ListSIGN::registerHandler(SignalId signal, SignalHandler handler)
{
	if (signal >= 0 && signal < numOfSignals)
	{
		Elem *temp = new Elem(signal, handler);
		if (!length)
		{
			head = tail = temp;
		}
		else
		{
			Elem *curr = head;
			Elem *prev = 0;
			if (signalPresentCounter[signal])
			{
				while (signal != curr->signalId)
				{
					prev = curr;
					curr = curr->next;
				}
				int offset = signalPresentCounter[signal];
				while (offset--)
				{
					prev = curr;
					curr = curr->next;
				}
				if (prev == tail)
				{
					tail = tail->next = temp;
				}
				else
				{
					temp->next = curr;
					prev->next = temp;
				}
			}
			else
			{
				int nextSignalId = 0;
				for (int i = signal - 1; i > 0; i--)
					if (signalPresentCounter[i])
					{
						nextSignalId = i;
						break;
					}
				while (nextSignalId != curr->signalId)
				{
					prev = curr;
					curr = curr->next;
				}
				int offset = signalPresentCounter[nextSignalId];
				while (offset--)
				{
					prev = curr;
					curr = curr->next;
				}
				if (prev == tail)
				{
					tail = tail->next = temp;
				}
				else
				{
					temp->next = curr;
					prev->next = temp;
				}
			}
		}
		length++;
		signalPresentCounter[signal]++;
	}
}

void ListSIGN::unregisterAllHandlers(SignalId id)
{
	if (signalPresentCounter[id])
	{
		int handlersToDelete = signalPresentCounter[id];
		Elem *curr = head;
		Elem *prev = 0;
		while (id != curr->signalId)
		{
			prev = curr;
			curr = curr->next;
		}
		while (handlersToDelete--)
		{
			length--;
			if (!length)
			{
				head = tail = 0;
				delete curr;
				break;
			}
			else if (curr == head)
			{
				head = head->next;
				delete curr;
				curr = head;
				continue;
			}
			else if (curr == tail)
			{
				tail = prev;
				delete curr;
				continue;
			}
			else
			{
				prev->next = curr->next;
				delete curr;
				curr = prev->next;
			}
		}
	}
	signalPresentCounter[id] = 0;
}

void ListSIGN::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
	if (signalPresentCounter[id])
	{
		Elem *curr = head;

		while (id != curr->signalId)
		{
			curr = curr->next;
		}

		int offset = signalPresentCounter[id];

		while (curr->signalHandler != hand1 && curr->signalHandler != hand2 && offset)
		{
			curr = curr->next;
			offset--;
			if (!offset)
			{
				return;
			}
		}

		SignalHandler firstFound = curr->signalHandler;

		if (firstFound == hand1)
		{
			Elem *tmp = curr->next;
			offset--;
			while (tmp->signalHandler != hand2 && offset)
			{
				tmp = tmp->next;
				offset--;
				if (!offset)
				{
					return;
				}
			}
			SignalHandler swap = curr->signalHandler;
			curr->signalHandler = tmp->signalHandler;
			tmp->signalHandler = swap;
		}
		else
		{
			Elem *tmp = curr->next;
			offset--;
			while (tmp->signalHandler != hand1 && offset)
			{
				tmp = tmp->next;
				offset--;
				if (!offset)
				{
					return;
				}
			}
			SignalHandler swap = curr->signalHandler;
			curr->signalHandler = tmp->signalHandler;
			tmp->signalHandler = swap;
		}
	}
}

void ListSIGN::callSelectedHandlers(SignalId id)
{
	if (signalPresentCounter[id])
	{
		Elem *curr = head;

		while (id != curr->signalId)
		{
			curr = curr->next;
		}

		int offset = signalPresentCounter[id];

		while (offset--)
		{
			curr->signalHandler();
			curr = curr->next;
		}
	}
}
