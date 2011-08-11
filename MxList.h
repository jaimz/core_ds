//
//  MxList.h
//  core_ds
//
//  A sentinel-terminated, doubly-linked list.
//
//  Created by J O'Brien on 28/08/2008
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxList_h
#define core_ds_MxList_h

#include "MxStatus.h"
#include "MxFunctions.h"


// TODO: This is only exposed in the header because MxHashtable
// uses it  directly - find a way around this.
typedef struct _MxListNode {
	struct _MxListNode *next;
	struct _MxListNode *prev;
	
	void *data;
} MxListNode, *MxListNodeRef;


typedef struct _MxList {
	MxListNodeRef sentinel;
    
	MxFreeFunction itemFree;
	MxEqualsFunction itemEquals;
    
	int count;
} MxList, *MxListRef;



/// Dynamically allocate a new list
MxListRef MxListCreate(void);
MxListRef MxListCreateWithFunctions(MxFreeFunction free, MxEqualsFunction equals);


/// Initialise a previously allocated list
MxStatus MxListInit(MxListRef list);
MxStatus MxListInitWithFunctions(MxListRef list, MxFreeFunction free, MxEqualsFunction equals);


/// Wipe a list structure - i.e. free and reset the internal state
MxStatus MxListWipe(MxListRef list);

/// Wipe a list and reclaim the memory used by it.
/// NOTE: the data held _in_ the list is not freed
MxStatus MxListDelete(MxListRef list);

/// Append data to the end of a list
MxStatus MxListAppend(MxListRef list, void *data);

/// Insert data at the front of a list
MxStatus MxListPush(MxListRef list, void *data);

/// Insert 'data' after the node containing 'after'
MxStatus MxListInsertAfter(MxListRef list, void *after, void *data);

/// Insert 'data' before the node containing 'before'
MxStatus MxListInsertBefore(MxListRef list, void *before, void *data);

/// Insert 'data' at index 'index
MxStatus MxListInsertAtIndex(MxListRef list, int index, void *data);

/// Remove the node at 'index' returning the data in the node in *result
MxStatus MxListRemoveAtIndex(MxListRef list, int idx, void **result);

/// Remove _all_ instances of 'data' from the list
MxStatus MxListRemove(MxListRef list, void *data);

/// Remove the node at the head of the list and return the data in it
MxStatus MxListPop(MxListRef list, void **result);

/// Remove the node at the tail of the list and return the data in it
MxStatus MxListDequeue(MxListRef list, void **result);

/// Remove every item in the list
MxStatus MxListClear(MxListRef list);

/// Iterate forward through the list calling 'callback' on each iteration
MxStatus MxListIterateForward(MxListRef list, MxIteratorCallback callback, void *state);

/// Iterate backward through the list calling 'callback' on each iteration
MxStatus MxListIterateBackward(MxListRef list, MxIteratorCallback callback, void *state);

/// Iterate through the list, removing nodes for which the callback returns non-zero
MxStatus MxListFilter(MxListRef list, MxIteratorCallback callback, void *state);

/// Append a map of the given list to a new list, the contents of the mapped list will be the elements
/// of the given list that satisfy the predicate defined by 'callback'
MxStatus MxListMap(MxListRef list, MxIteratorCallback callback, void *state, MxListRef result);

/// Find the index of an item using the supplied equality function - if equalsFunction is NULL the list's
/// compare function is used, if the list does not have a compare function "==" is used
MxStatus MxListFindIndex(MxListRef list, void *lookFor, MxEqualsFunction equalsFunction, int *result);

/// Find the item matching "lookFor" according to the supplied equality function - if the equality function is NULL
/// the list's equality function is used, if that is NULL "==" is used
MxStatus MxListFindItem(MxListRef list, void *lookFor, MxEqualsFunction equalsFunction, void **result);

/// Return the number of nodes in the list
int MxListGetCount(MxListRef list);

#endif
