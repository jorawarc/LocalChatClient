#include "List.h"
#include "Node.h"
#include <stdio.h>
#include <stdlib.h>



// -- Global Variables -- //

int usedHeads;
int usedNodes;

LIST headList[MAX_HEADS];  // List of Heads
Node nodeList[MAX_NODES];  // List of Nodes

LIST* freeHeadGlb;   // Points to free head
Node* freeNodeGlb;   // Points to free node


/* Makes a new, empty list, and returns its reference on 
   success. Returns a NULL pointer on failure. */
LIST* ListCreate(){
	LIST* tmp = NULL;
	int i, k;

	if(MAX_HEADS == 0){
		return NULL;
	}

	if(usedHeads == MAX_HEADS){
		return NULL;
	}

	if(usedHeads == 0 && MAX_NODES != 0){			// First instance of List object //
		headList[MAX_HEADS-1].freeHead = NULL; 			
		nodeList[MAX_NODES-1].next = NULL;
		nodeList[MAX_NODES-1].last = NULL;

		freeHeadGlb = &(headList[0]);	   			// First avalible head is beginning of list
		freeNodeGlb = &(nodeList[0]);



		for(i=0; i<MAX_HEADS-1;i++){					// Chain all heads together
			headList[i].freeHead = &(headList[i+1]);	// Track heads as a stack, without searching 
		}

		for(k=0; k<MAX_NODES-1;k++){					// Chain nodes together
			nodeList[k].next = &(nodeList[k+1]);
			nodeList[k].last = NULL;
		}
	}


	tmp = freeHeadGlb;

	if(tmp != NULL){
		tmp->beyond = 0;
		tmp->currentNode = NULL;
		tmp->head = NULL;
		tmp->tail = NULL;
		tmp->elementCount = 0;

		freeHeadGlb = freeHeadGlb->freeHead; 
		usedHeads = usedHeads + 1;
    	return tmp;
	}


	return NULL;

}


/* Makes Nodes if they are avalible */
Node* NodeCreate(void* item, Node* forw, Node* prev){
	Node* tmp;
	if(MAX_NODES == 0 || MAX_HEADS == 0){
		return NULL;
	}
	if(usedNodes < MAX_NODES){
		tmp = freeNodeGlb;
		freeNodeGlb = tmp->next;

		tmp->data = item;
		tmp->next = forw;
		tmp->last = prev;

		usedNodes += 1;
		return tmp;
	}
	return NULL;
}


/* Returns the number of items in list. */
int ListCount(LIST* l){
	if(l != NULL || MAX_NODES == 0){
		return l->elementCount;
	}
	else{
		return 0;
	}
}


/* Returns a pointer to the first item in list and makes the first item
   the current item */
void* ListFirst(LIST* l){
	if(l == NULL || MAX_NODES == 0){
		return NULL;
	}
	void* tmp = NULL;
	if(l->elementCount != 0){	
		tmp = l->head->data;
		l->currentNode = l->head; 
		l->beyond = 0;

	}
	return tmp;  
}


/* Returns a pointer to the last item in the list and makes the last item the current one. */
void* ListLast(LIST* l){
	if(l == NULL || MAX_NODES == 0 || MAX_HEADS == 0){
		return NULL;
	}
	void* tmp = NULL;
	if(l->elementCount != 0){	
		tmp = l->tail->data;
		l->currentNode = l->tail;
		l->beyond = 0;
	}
	return tmp;
}

/* Advances list's current item by one, and returns a 
   pointer to the new current item. If this operation advances the current item beyond 
   the end of the list, a NULL pointer is returned. */
void *ListNext(LIST* l){
	if(l == NULL || MAX_NODES == 0){
		return NULL;
	}
	void* tmp = NULL;
	if(l->currentNode != NULL){
		if(l->currentNode->next != NULL && l->beyond == 0){
			tmp = l->currentNode->next->data;
			l->currentNode = l->currentNode->next;
			l->beyond = 0;
		}
		else if(l->beyond == -1){
			l->beyond = 0;
			return l->currentNode->data;
		}
		else{
			l->beyond = 1;
		}
	}
	return tmp;
}


/* Backs up list's current item by one, and returns a 
   pointer to the new current item. If this operation backs up the current item beyond 
   the start of the list, a NULL pointer is returned. */
void *ListPrev(LIST* l){
	if(l == NULL || MAX_NODES == 0){
		return NULL;
	}
	void* tmp = NULL;
	if(l->currentNode->last != NULL && l->beyond == 0){
		tmp = l->currentNode->last->data;
		l->currentNode = l->currentNode->last;
		l->beyond = 0;
	}
	else if(l->beyond == 1){
		l->beyond = 0;
		return l->currentNode->data;
	}
	else{
		l->beyond = -1;
	}
	return NULL;
}


/* Returns a pointer to the current item in list. */
void *ListCurr(LIST* l){
	if(l != NULL && l->currentNode != NULL && l->beyond == 0)
		return l->currentNode->data;

	return NULL;
}

/* Adds the new item to list directly after the 
   current item, and makes item the current item. If the current pointer is before the 
   start of the list, the item is added at the start. If the current pointer is beyond the 
   end of the list, the item is added at the end. Returns 0 on success, -1 on failure. */
int ListAdd(LIST* l, void* item){
	if(l == NULL || usedNodes == MAX_NODES || MAX_NODES == 0){
		return -1;
	}

	int c;
	Node* tmp;
	// Empty List
	if(l->beyond == 0){
		if(l->elementCount == 0|| l->currentNode == l->tail){
			c = ListAppend(l,item);
			if(c == 0){
				l->beyond = 0;
			}
			return c;
		}
		else{
			tmp = NodeCreate(item,l->currentNode->next,l->currentNode);
			if(tmp == NULL){
				return -1;
			}
			l->currentNode->next = tmp;
			l->currentNode->next->next->last = tmp;
			l->currentNode = tmp;
			l->elementCount += 1;
			return 0;
		}
	}
	else if(l->beyond == 1){
		c = ListAppend(l,item);
		if(c == 0){
			l->beyond = 0;
		}
		return c;

	}
	else if(l->beyond == -1){
		c = ListPrepend(l,item);
		if(c == 0){
			l->beyond = 0;
		}
		return c;
	}
	return -1;

}

/* Adds item to list directly before the current 
   item, and makes the new item the current one. If the current pointer is before the 
   start of the list, the item is added at the start. If the current pointer is beyond the 
   end of the list, the item is added at the end. Returns 0 on success, -1 on failure. */
int ListInsert(LIST* l, void* item){

	if(l == NULL || usedNodes == MAX_NODES){
		return -1;
	}

	Node* tmp = NULL;
	int c;
	if(l->beyond == 0){
		if(l->elementCount == 0 || l->currentNode == l->head){
			c = ListPrepend(l,item);
			if(c == 0){
				l->beyond = 0;
			}
			return c;
		}
		else{
			tmp = NodeCreate(item,l->currentNode,l->currentNode->last);
			if(tmp == NULL){
				return -1;
			}
			l->currentNode->last->last->next = tmp;
			l->currentNode->last = tmp;
			l->currentNode = tmp;
			l->elementCount += 1;
			return 0;
		}
	}
	else if(l->beyond == 1){
		c = ListAppend(l,item);
		if(c == 0){
			l->beyond = 0;
		}
		return c;
	}
	else if(l->beyond == -1){
		c = ListPrepend(l, item);
		if(c == 0){
			l->beyond = 0;
		}
		return c;
	}
	return -1;
}


/* Adds item to the end of list, and makes the 
   new item the current one. Returns 0 on success, -1 on failure. */
int ListAppend(LIST* l, void* item){
	Node* tmp;

	if(l == NULL){
		return -1;
	}

	// Empty List Case
	if(l->elementCount == 0){
		tmp = NodeCreate(item,NULL,NULL);
		if(tmp == NULL){
			return -1;
		}
		l->head = tmp;
		l->tail = tmp;
		l->currentNode = tmp;
		l->elementCount += 1;
		l->beyond = 0;

		return 0;
	}
	else{
		tmp = NodeCreate(item,NULL,l->tail);
		if(tmp == NULL){
			return -1;
		}
		l->tail->next = tmp;
		l->tail = tmp;
		l->currentNode = tmp;
		l->elementCount += 1;
		l->beyond = 0;

		return 0;
	}

	return -1;
}


/* Adds item to the front of list, and makes 
   the new item the current one. Returns 0 on success, -1 on failure. */
int ListPrepend(LIST* l, void* item){
	if(l == NULL){
		return -1;
	}

	Node* tmp;
	if(l->elementCount == 0){
		return ListAppend(l,item);
	}
	else{
		tmp = NodeCreate(item,l->head,NULL);
		if(tmp == NULL){
			return -1;
		}
		l->head->last = tmp;
		l->head = tmp;
		l->currentNode = tmp;
		l->elementCount += 1;
		l->beyond = 0;
		return 0;
	}
	return -1;
}


/* Return current item and take it out of list. Make 
   the next item the current one. */
void *ListRemove(LIST* l){
	if(l->elementCount == 0 || l == NULL || MAX_NODES == 0){
		return NULL;
	}
	if(l->beyond == 0){
		void* retItem = l->currentNode->data;
		if(l->elementCount == 1){
			l->head = NULL;
			l->tail = NULL;
			l->currentNode->next = freeNodeGlb;
			l->currentNode->last = NULL;
			freeNodeGlb = l->currentNode;
			l->currentNode = NULL;

			l->elementCount -= 1;
			l->freeHead = freeHeadGlb;
			freeHeadGlb = l;
			usedHeads -=1;
			usedNodes -= 1;
		}

		// if removed node = head
		else if(l->currentNode == l->head){
			l->head = l->head->next;
			l->head->last = NULL;

			l->currentNode->next = freeNodeGlb;
			l->currentNode->last = NULL;
			freeNodeGlb = l->currentNode;


			l->currentNode = l->head;
			l->elementCount -= 1;
			usedNodes -= 1;
		}
		else if(l->currentNode == l->tail){
			l->tail = l->tail->last;
			l->tail->next = NULL;

			l->currentNode->next = freeNodeGlb;
			l->currentNode->last = NULL;
			freeNodeGlb = l->currentNode;

			l->currentNode = l->head;
			l->elementCount -= 1;
			usedNodes -= 1;
		}
		else{
			Node* prev = l->currentNode->last;
			Node* forw = l->currentNode->next;

			prev->next = forw;
			forw->last = prev;

			l->currentNode->next = freeNodeGlb;
			l->currentNode->last = NULL;
			freeNodeGlb = l->currentNode;

			l->currentNode = forw;
			l->elementCount -= 1;
			usedNodes -= 1;
		}
		//printf("REMOVED:::%p ::::::: %d\n",retItem,retItem );
		return retItem;
	}
	return NULL;


}


/* Adds list2 to the end of list1. The 
   current pointer is set to the current pointer of list1. List2 no longer exists after the 
   operation. */
void ListConcat(LIST* l1, LIST* l2){
	if(l1 == NULL || l2 == NULL || MAX_HEADS < 2){
		return;
	}

	if(l1->elementCount == 0){
		l1->freeHead = l2->freeHead;
		l1->elementCount = l2->elementCount;
		l1->head = l2->head;
		l1->tail = l2->tail;
		l1->currentNode = l2->currentNode;
		l1->beyond = l2->beyond;
	}
	else{
		l1->tail->next = l2->head;
		l1->tail = l2->tail;
		l2->head->last = l1->tail;
		l1->elementCount += l2->elementCount;

		if(l2->beyond == 1 || l1->beyond == 1){
			l1->currentNode = l1->tail;
			l1->beyond = 1;
		}
		else if(l2->beyond == -1 || l1->beyond == -1){
			l1->beyond = -1;
			l1->currentNode = l1->head;
		}

		l2->head = NULL;
		l2->tail = NULL;
		l2->currentNode = NULL;
		l2->beyond = 0;

		l2->freeHead = freeHeadGlb;
		freeHeadGlb = l2;
		usedHeads -= 1;
	}

	return;
}


/* Delete list. itemFree is a pointer to a 
   routine that frees an item. It should be invoked (within ListFree) as: 
   (*itemFree)(itemToBeFreed); */
void ListFree(LIST* l, void* (*itemFree) (void *)){
	if(l == NULL || MAX_HEADS == 0){
		return;
	}

	if(MAX_NODES != 0){
		Node* cur = l->head;
		void* itemToBeFreed = l->head->data;
		while(cur != NULL){
			(*itemFree) (itemToBeFreed);
			cur = cur->next;
		}
	}

	l->elementCount = 0;
	l->head = NULL;
	l->tail = NULL;
	l->currentNode = NULL;
	l->beyond = 0;
	l->freeHead = freeHeadGlb;
	freeHeadGlb = l;
	usedHeads-= 1;
	return;

}


/* Return last item and take it out of list. Make the 
   new last item the current one. */
void *ListTrim(LIST* l){
	if(l == NULL || MAX_NODES == 0){
		return NULL;
	}
	if(l->elementCount == 0){
		return NULL;
	}

	//void* item = l->tail->data = item;

	ListLast(l);
	return ListRemove(l);
	
}


/* Searches list starting at the current item until the end is reached or a match is 
   found. In this context, a match is determined by the comparator parameter. This 
   parameter is a pointer to a routine that takes as its first argument an item pointer, 
   and as its second argument comparisonArg. Comparator returns 0 if the item and 
   comparisonArg don't match, or 1 if they do. Exactly what constitutes a match is 
   up to the implementor of comparator. If a match is found, the current pointer is 
   left at the matched item and the pointer to that item is returned. If no match is 
   found, the current pointer is left beyond the end of the list and a NULL pointer is 
   returned. */


void *ListSearch(LIST* l, int (*comparator)(const void*, const void*), void *comparisonArg){
	if(l == NULL || l->currentNode == NULL || l->beyond == 1 || MAX_NODES == 0){
		return NULL;
	}
	Node* cur = l->currentNode;
	int found = 0;

	while(cur != NULL && found == 0){
		//printf("currentNode->data %d\n",*(int*) l->currentNode->data );
		found = (*comparator)(cur->data, comparisonArg);
		//printf("Found == %d\n",found );

		if(found == 1){
			l->currentNode = cur;
			return l->currentNode->data;
		}
		cur = cur->next;
	}
	l->currentNode = l->tail;
	l->beyond = 1;
	return NULL;
}

