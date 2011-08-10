//
//  MxArrayList.h
//  core_ds
//
//  Dynamic array implementation
//
//  Created by J O'Brien on 09/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxArrayList_h
#define core_ds_MxArrayList_h


#include "MxStatus.h"
#include "MxFunctions.h"

#define MxArrayListDefaultCapacity (31)
#define MxArrayListExpansionFactor (2)

typedef struct _MxArrayList {
	size_t capacity;
	size_t count;
	
	MxFreeFunction itemFree;
	MxEqualsFunction itemEquals;
	
	void **items;
} MxArrayList, *MxArrayListRef;


MxArrayListRef MxArrayListCreate(void);
MxArrayListRef MxArrayListCreateWithFunctions(MxFreeFunction itemFree, MxEqualsFunction itemEquals);
MxArrayListRef MxArrayListCreateWithCapacity(size_t capacity);
MxArrayListRef MxArrayListCreateWithCapacityAndFunctions(size_t capacity, MxFreeFunction itemFree, MxEqualsFunction itemEquals);

MxStatus MxArrayListInit(MxArrayListRef list);
MxStatus MxArrayListInitWithCapacity(MxArrayListRef list, size_t capacity);
MxStatus MxArrayListInitWithFunctions(MxArrayListRef list, MxFreeFunction itemFree, MxEqualsFunction itemEquals);
MxStatus MxArrayListInitWithCapacityAndFunctions(MxArrayListRef list, size_t capacity, MxFreeFunction itemfree, MxEqualsFunction itemEquals);


MxStatus MxArrayListWipe(MxArrayListRef list);
MxStatus MxArrayListDelete(MxArrayListRef list);


MxStatus MxArrayListAppend(MxArrayListRef list, const void *item);
MxStatus MxArrayListPush(MxArrayListRef list, const void *item);
MxStatus MxArrayListInsertAt(MxArrayListRef list, const void *item, int index);
MxStatus MxArrayListReplaceAt(MxArrayListRef list, const void *item, int index);


MxStatus MxArrayListItemAt(MxArrayListRef list, int index, void **result);
MxStatus MxArrayListPop(MxArrayListRef list, void **result);
MxStatus MxArrayListRemoveAt(MxArrayListRef list, int index, void **removed);
MxStatus MxArrayListClear(MxArrayListRef list);


MxStatus MxArrayListIterate(MxArrayListRef list, MxIteratorCallback callback, void *state);
MxStatus MxArrayListIterateBackward(MxArrayListRef list, MxIteratorCallback, void *state);



size_t MxArrayListGetCount(MxArrayListRef list);
int MxArrayListIndexOf(MxArrayListRef list, void *item);


#endif
