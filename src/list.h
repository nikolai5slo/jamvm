
struct linkedlistitem{
	LinkedListItem* next;
	LinkedListItem* prev;
	void* value;
} LinkedListItem;

struct linkedlist{
	LinkedListItem* first;
	int count;
} LinkedList;

void linkedListInit(LinkedList*);
void linkedListAdd(LinkedList*,void*);
void linkedListRemove(LinkedList*,void*);


