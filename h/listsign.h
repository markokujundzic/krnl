#ifndef _listsign_h_
#define _listsign_h_

#ifndef numOfSignals
#define numOfSignals 16
#endif

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class ListSIGN
{
public:
	struct Elem
	{
		SignalHandler signalHandler;
		SignalId signalId;
		Elem *next;
		Elem(SignalId signalId, SignalHandler signalHandler = 0) : signalId(signalId), signalHandler(signalHandler), next(0)
		{
		}
	};

	Elem *head, *tail;
	int length;

	int signalPresentCounter[numOfSignals];

	ListSIGN();
	~ListSIGN();

	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void callSelectedHandlers(SignalId id);

	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);

	void push_front(SignalId id, SignalHandler handler = 0);
	void push_back(SignalId id, SignalHandler handler = 0);

	SignalId pop_front();
	SignalId pop_back();
};

#endif
