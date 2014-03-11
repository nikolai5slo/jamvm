#include "list.h"

LinkedList* newLinkedList(){
	LinkedList* list=malloc(sizeof(LinkedList));	
	list->first=NULL;
	list->count=0;
}
void linkedListAdd(LinkedList* list,void* value){
	LinkedListItem* item=list->first;
	if(item==NULL){
		list->first=malloc(sizeof(LinkedListItem));
		list->first->prev=NULL;
		item=list->first;
	}else{ 
		while(item->next!=NULL) item=item->next;
		item->next=malloc(sizeof(LinkedListItem));
		item->next->prev=item;
		item=item->next;
	}
	item->value=value;
	item->next=NULL;

	list->count++;
}
int linkedListRemove(LinkedList* list,void* value){
	LinkedList* item=list->first;	
	while(item!=NULL){
		if(item->value==value){
			if(item->prev==NULL) list->first=item->next;
			else item->prev->next=item->next;
			item->next->prev=item->prev;
			free(item);
			list->count--;
			return 1;
		}
	}
	return 0;
}

