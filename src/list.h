typedef struct linkedlistitem LinkedListItem;
struct linkedlistitem{
	LinkedListItem* next;
	LinkedListItem* prev;
	void* value;
};

struct linkedlist{
	LinkedListItem* first;
	int count;
} typedef LinkedList;

extern LinkedList* newLinkedList();
extern void linkedListAdd(LinkedList*,void*);
extern int linkedListRemove(LinkedList*,void*);


