/*
 * List.h
 *	
 * Prototype Declaractions of LIST ADT
 * Data collection class used to store lists, which can store multiple nodes	
 *
 * Author: Jorawar Chana
 * Data Last Modified: Jan. 8, 2018 
 */

#ifndef LIST_H
#define LIST_H

#define MAX_HEADS 100
#define MAX_NODES 5000

#include "Node.h"
#include <stdio.h>
#include <stdlib.h>



// LIST type declaration
typedef struct _list {

   int elementCount;
   struct _list* freeHead;     // Points to a free head in the headList // acts as singly linked list

   Node* currentNode;          // Points to 'Current item' 
   Node* head;
   Node* tail;

   int beyond;


} LIST;


/* Makes a new, empty list, and returns its reference on 
   success. Returns a NULL pointer on failure. */
LIST* ListCreate();


/* Returns the number of items in list. */
int ListCount(LIST* l);


/* Returns a pointer to the first item in list and makes the first item
   the current item */
void* ListFirst(LIST* l);


/* Returns a pointer to the last item in the list and makes the last item the current one. */
void* ListLast(LIST* l);


/* Advances list's current item by one, and returns a 
   pointer to the new current item. If this operation advances the current item beyond 
   the end of the list, a NULL pointer is returned. */
void *ListNext(LIST* l);


/* Backs up list's current item by one, and returns a 
   pointer to the new current item. If this operation backs up the current item beyond 
   the start of the list, a NULL pointer is returned. */
void *ListPrev(LIST* l);


/* Returns a pointer to the current item in list. */
void *ListCurr(LIST* l);


/* Adds the new item to list directly after the 
   current item, and makes item the current item. If the current pointer is before the 
   start of the list, the item is added at the start. If the current pointer is beyond the 
   end of the list, the item is added at the end. Returns 0 on success, -1 on failure. */
int ListAdd(LIST* l, void* item);


/* Adds item to list directly before the current 
   item, and makes the new item the current one. If the current pointer is before the 
   start of the list, the item is added at the start. If the current pointer is beyond the 
   end of the list, the item is added at the end. Returns 0 on success, -1 on failure. */
int ListInsert(LIST* l, void* item);


/* Adds item to the end of list, and makes the 
   new item the current one. Returns 0 on success, -1 on failure. */
int ListAppend(LIST* l, void* item);


/* Adds item to the front of list, and makes 
   the new item the current one. Returns 0 on success, -1 on failure. */
int ListPrepend(LIST* l, void* item);


/* Return current item and take it out of list. Make 
   the next item the current one. */
void *ListRemove(LIST* l);


/* Adds list2 to the end of list1. The 
   current pointer is set to the current pointer of list1. List2 no longer exists after the 
   operation. */
void ListConcat(LIST* l1, LIST* l2);


/* Delete list. itemFree is a pointer to a 
   routine that frees an item. It should be invoked (within ListFree) as: 
   (*itemFree)(itemToBeFreed); */
void ListFree(LIST* l, void* (*itemFree) (void *));


/* Return last item and take it out of list. Make the 
   new last item the current one. */
void *ListTrim(LIST* l);


/* Searches list starting at the current item until the end is reached or a match is 
   found. In this context, a match is determined by the comparator parameter. This 
   parameter is a pointer to a routine that takes as its first argument an item pointer, 
   and as its second argument comparisonArg. Comparator returns 0 if the item and 
   comparisonArg don't match, or 1 if they do. Exactly what constitutes a match is 
   up to the implementor of comparator. If a match is found, the current pointer is 
   left at the matched item and the pointer to that item is returned. If no match is 
   found, the current pointer is left beyond the end of the list and a NULL pointer is 
   returned. */
void *ListSearch(LIST* l, int (*comparator)(const void*, const void*), void *comparisonArg);

#endif
