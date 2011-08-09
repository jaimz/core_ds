//
//  MxList.c
//  core_ds
//
//  Created by J O'Brien on 07/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "MxStatus.h"
#include "MxFunctions.h"
#include "MxList.h"


static MxListNodeRef MxListNodeCreate(void *data);
static void MxListNodeInsertBefore(MxListNodeRef before, MxListNodeRef newNode);
static void MxListNodeInsertAfter(MxListNodeRef after, MxListNodeRef newNode);
static MxListNodeRef MxListNodeLookup(MxListRef list, void *data);
static void MxListNodeRemove(MxListNodeRef node);


MxStatus MxListInit(MxListRef list) {
	list->sentinel = MxListNodeCreate(NULL);
	if (list->sentinel == NULL)
		return MxStatusNoMemory ;
    
	list->sentinel->next = list->sentinel;
	list->sentinel->prev = list->sentinel;
    
	list->count = 0;
    
	list->itemEquals = MxDefaultEqualsFunction;
	list->itemFree = MxDefaultFreeFunction;
    
	return MxStatusOK;
}


MxStatus MxListInitWithFunctions(MxListRef list, MxFreeFunction free, MxEqualsFunction equals) {
	int result = MxListInit(list);
	if (result == MxStatusOK) {
		list->itemEquals = equals;
		list->itemFree = free;
	}
    
	return MxStatusOK;
}


MxListRef MxListCreate(void) {
    MxListRef list = (MxListRef)malloc(sizeof(MxList));
    if (list != NULL) {
        if (MxListInit(list) != MxStatusOK) {
            free(list);
            list = NULL;
        }
    }
    
    return list;
}



MxListRef MxListCreateWithFunctions(MxFreeFunction free, MxEqualsFunction equals) {
	MxListRef result = (MxListRef)malloc(sizeof(MxList));
    
	if (result != NULL) {
		if (MxListInitWithFunctions(result, free, equals) != MxStatusOK) {
			free(result);
			result = NULL;
		}
	}
    
	return result;
}


MxStatus MxListClear(MxListRef list)
{
	if (list == NULL)
		return MxStatusNullArgument;
    
	MxListNodeRef curr = list->sentinel->next;
	MxListNodeRef tmp = NULL;
	
	if (list->itemFree)
	{
		while (curr != list->sentinel)
		{
			list->itemFree(curr->data);
			curr = curr->next;
		}
	}
	
	/* Destoy the nodes */
	curr = curr->next;
	while (curr != list->sentinel) {
		tmp = curr->next;
		free(curr);
		curr = tmp;
	}
	
	list->sentinel->next = list->sentinel;
	list->sentinel->prev = list->sentinel;
	
	list->count = 0;
	return MxStatusOK;
}

MxStatus MxListWipe(MxListRef list) {
	if (list == NULL)
		return MxStatusNullArgument;
    
	MxListNodeRef curr = list->sentinel->next;
	MxListNodeRef tmp = NULL;
    
	/*	Destroy the referenced data if there's a free
     function */
	if (list->itemFree) {
		while (curr != list->sentinel) {
			list->itemFree(curr->data);
			curr = curr->next;
		}
	}
    
	/* Destoy the nodes */
	curr = curr->next;
	while (curr != list->sentinel) {
		tmp = curr->next;
		free(curr);
		curr = tmp;
	}
    
	/* Destroy the sentinel */
	free(list->sentinel);
	
	/* Just in case */
	memset(list, 0, sizeof(MxList));
    
	return MxStatusOK;
}

MxStatus MxListDelete(MxListRef list) {
	if (list == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxListWipe(list);
	
	if (result == MxStatusOK)
		free(list);
	
	return result;
}


static MxListNodeRef MxListNodeCreate(void *data) {
	MxListNodeRef result = malloc(sizeof(MxListNode));
	if (result != NULL) {
		result->next = NULL;
		result->prev = NULL;
		result->data = data;
	}
    
	return result;
}



MxStatus MxListAppend(MxListRef list, void *data) {
	if (!list)
		return MxStatusNullArgument;
    
	if (!list->sentinel)
		return MxStatusBadArgument;
    
    
	MxListNodeRef newNode = MxListNodeCreate(data);
	if (!newNode)
		return MxStatusNoMemory;
    
	MxListNodeInsertBefore(list->sentinel, newNode);
    
	list->count++;
    
	return MxStatusOK;
}



MxStatus MxListPush(MxListRef list, void *data) {
	if (!list)
		return MxStatusNullArgument;
    
	if (!list->sentinel)
		return MxStatusBadArgument;
    
	MxListNodeRef newNode = MxListNodeCreate(data);
	if (!newNode)
		return MxStatusNoMemory;
    
	MxListNodeInsertAfter(list->sentinel, newNode);
    
	list->count++;
	return MxStatusOK;
}



MxStatus MxListInsertAfter(MxListRef list, void *after, void *data) {
	if (!list)
		return MxStatusNullArgument;
	
	if (!list->sentinel)
		return MxStatusBadArgument;
    
	
	MxListNodeRef afterNode = MxListNodeLookup(list, after);
	if (!afterNode) 
		return MxStatusNotFound;
    
	MxListNodeRef newNode = MxListNodeCreate(data);
	if (!newNode) 
		return MxStatusNoMemory;
    
	MxListNodeInsertAfter(afterNode, newNode);
    
	list->count++;
	return MxStatusOK;
}


MxStatus MxListInsertBefore(MxListRef list, void *before, void *data) {
	if (!list)
		return MxStatusNullArgument;
	
	if (!list->sentinel)
		return MxStatusBadArgument;
	
	MxListNodeRef beforeNode = MxListNodeLookup(list, before);
	if (beforeNode == NULL) 
		return MxStatusNotFound;
    
	MxListNodeRef newNode = MxListNodeCreate(data);
	if (newNode == NULL) 
		return MxStatusNoMemory;
    
	MxListNodeInsertBefore(beforeNode, newNode);
    
	list->count++;
	return MxStatusOK;
}


MxStatus MxListInsertAtIndex(MxListRef list, int index, void *data) {
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
    
	if (index < 0 || index >= list->count)
		return MxStatusIndexOutOfRange;
    
    
	MxListNodeRef newNode = MxListNodeCreate(data);
	if (newNode == NULL) 
		return MxStatusNoMemory;
    
	MxListNodeRef node = list->sentinel->next;
	for (int ctr = 0; ctr < index; ++ctr)
		node = node->next;
    
	MxListNodeInsertBefore(node, newNode);
    
	list->count++;
	return MxStatusOK;
}

MxStatus MxListRemoveAtIndex(MxListRef list, int index, void **result) {
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
    
	if (index < 0 || index >= list->count)
		return MxStatusIndexOutOfRange;
    
    
	MxListNodeRef node = list->sentinel->next;
	for (int ctr = 0; ctr < index; ++ctr) node = node->next;
    
	*result = node->data;
	MxListNodeRemove(node);
    
	list->count--;
	return MxStatusOK;
}

static void MxListNodeInsertBefore(MxListNodeRef before, MxListNodeRef newNode) {
	// TODO: assert before, newNode
	before->prev->next = newNode;
	newNode->prev = before->prev;
    
	before->prev = newNode;
	newNode->next = before;
}

static void MxListNodeInsertAfter(MxListNodeRef after, MxListNodeRef newNode) {
	after->next->prev = newNode;
	newNode->next = after->next;
    
	after->next = newNode;
	newNode->prev = after;
}

static MxListNodeRef MxListNodeLookup(MxListRef list, void *data) {
	MxListNodeRef result = NULL;
	MxListNodeRef curr = list->sentinel->next;
    
	while (curr != list->sentinel) {
		if (list->itemEquals)
		{
			if (list->itemEquals(curr->data, data)) {
				result = curr;
				break;
			}
		}
		else
		{
			if (curr->data == data)
			{
				result = curr;
				break;
			}
		}
		
		curr = curr->next;
	}
    
	
	return result;
}


MxStatus MxListRemove(MxListRef list, void *data) {
	
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
    
	MxListNodeRef toRemove = MxListNodeLookup(list, data);
	if (toRemove == NULL) 
		return MxStatusNotFound;
    
	if (list->itemFree != NULL)
	 	list->itemFree(toRemove->data);
    
	MxListNodeRemove(toRemove);
    
	list->count--;
	return MxStatusOK;
}

static void MxListNodeRemove(MxListNodeRef node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
    
	free(node);
}

/// Remove the node at the head of the list and return the data in it
MxStatus MxListDequeue(MxListRef list, void **result)
{
	if (list == NULL || result == NULL) return MxStatusNullArgument;
	if (list->sentinel == NULL) return MxStatusBadArgument;
    
	// If the list is already empty set the result to NULL
	*result = NULL;
	if (list->sentinel->prev == list->sentinel)
	{
		*result = NULL; // empty list
		return MxStatusOK;
	}
	
	*result = list->sentinel->prev->data;
	MxListNodeRemove(list->sentinel->prev);
	
	list->count--;
	return MxStatusOK;
}


/// Remove the node at the tail of the list and return the data in it
MxStatus MxListPop(MxListRef list, void **result)
{
	if (list == NULL || result == NULL) return MxStatusNullArgument;
	if (list->sentinel == NULL) return MxStatusBadArgument;
    
	*result = NULL;
    
	// If the list is already empty the result is null
	if (list->sentinel->next == list->sentinel)
		return MxStatusOK;
    
    
	*result = list->sentinel->next->data;
	MxListNodeRemove(list->sentinel->next);
	
	list->count--;
	return MxStatusOK;
}

int MxListIterateForward(MxListRef list, MxIteratorCallback callback, void *state) {
	MxListNodeRef curr;
	int result;
    
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
    
	curr = list->sentinel->next;
    
	result = 0;
	while (result == 0 && curr != list->sentinel) {
		result = callback(curr->data, state);
		curr = curr->next;
	}
    
	return result;
}

int MxListIterateBackward(MxListRef list, MxIteratorCallback callback, void *state) {
	MxListNodeRef curr;
	int result;
    
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
    
    
	curr = list->sentinel->next;
    
	result = 0;
	while (result == 0 && curr != list->sentinel) {
		result = callback(curr->data, state);
		curr = curr->prev;
	}
    
	return result;
}


MxStatus MxListFilter(MxListRef list, MxIteratorCallback callback, void *state) {
	MxListNodeRef curr, toRemove;
	MxFreeFunction ff;
    
	if (list == NULL)
		return MxStatusNullArgument;
	if (list->sentinel == NULL)
		return MxStatusBadArgument;
	curr = list->sentinel->next;
    
	toRemove = NULL;
	ff = list->itemFree;
    
	while (curr != list->sentinel) {
		if (callback(curr->data, state)) toRemove = curr;
		else toRemove = NULL;
		curr = curr->next;
        
		if (toRemove) {
			if (ff) ff(toRemove->data);
			MxListNodeRemove(toRemove);
			list->count--;
		}	
	}
    
	return MxStatusOK;
}


MxStatus MxListMap(MxListRef list, MxIteratorCallback callback, void *state, MxListRef result)
{
	if (list == NULL || result == NULL) return MxStatusNullArgument;
	if (list->sentinel == NULL) return MxStatusBadArgument;
	
	MxListNodeRef curr = list->sentinel->next;
	MxStatus status = MxStatusOK;
	
	while (curr != list->sentinel)
	{
		if (callback(curr->data, state))
			if ((status = MxListAppend(result, curr->data)) != MxStatusOK)
				break;
		
		curr = curr->next;
	}
	
	return MxStatusOK;
}


/// Find the index of an item using the supplied equality function - if equalsFunction is NULL the list's
/// compare function is used, if the list does not have a compare function "==" is used
MxStatus MxListFindIndex(MxListRef list, void *lookFor, MxEqualsFunction equalsFunction, int *result)
{
	if (list == NULL || lookFor == NULL || result == NULL)
		return MxStatusNullArgument;
	
	if (!list->sentinel)
		return MxStatusBadArgument;
	
	*result = -1;
	MxStatus status = MxStatusNotFound;
	MxListNodeRef curr = list->sentinel->next;
	MxEqualsFunction eq = equalsFunction;
	if (!eq) eq = list->itemEquals;
	
	int idx = 0;
	while (curr != list->sentinel)
	{
		if (eq)
			if (eq(lookFor, curr->data))
				break;
            else
                if (lookFor == curr->data)
                    break;
        
		idx++;
	}
	
	if (curr != list->sentinel)
	{
		*result = idx;
		status = MxStatusOK;
	}
	
	return status;
}


/// Find the item matching "lookFor" according to the supplied equality function - if the equality function is NULL
/// the list's equality function is used, if that is NULL "==" is used
MxStatus MxListFindItem(MxListRef list, void *lookFor, MxEqualsFunction equalsFunction, void **result)
{
	if (list == NULL || lookFor == NULL || result == NULL)
		return MxStatusNullArgument;
	
	if (!list->sentinel)
		return MxStatusBadArgument;
	
	MxStatus status = MxStatusNotFound;
	MxListNodeRef curr = list->sentinel->next;
	MxEqualsFunction eq = equalsFunction;
	if (!eq) eq = list->itemEquals;
	*result = NULL;
    
	while (curr != list->sentinel && !(*result))
	{
		if (eq && eq(curr->data, lookFor))
			*result = curr->data;
		else
			if (curr->data == lookFor)
				*result = curr->data;
	}
	
	if (*result)
		status = MxStatusOK;
	
	return status;
}


int MxListGetCount(MxListRef list) {
	if (list == NULL)
	 	return MxStatusNullArgument;
    
	return list->count;
}