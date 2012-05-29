//
//  MxArrayList.c
//  core_ds
//
//  Created by J O'Brien on 09/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "MxStatus.h"
#include "MxFunctions.h"

#include "MxArrayList.h"


// expand the internal array
static inline MxStatus ExpandIfNeeded(MxArrayListRef list)
{
	/* The "if needed" bit... */
	if (list->count < list->capacity)
		return MxStatusOK;
    
	size_t newSize = (list->capacity * MxArrayListExpansionFactor) * sizeof(void *);	
	void **newItems;
    
	newItems = (void **) realloc((void *)(list->items), newSize);
	if (newItems == NULL)
		return MxStatusNoMemory;
	
	list->items = newItems;
	list->capacity *= MxArrayListExpansionFactor;
	
	return MxStatusOK;
}



MxArrayListRef MxArrayListCreate(void)
{
	MxArrayListRef result = malloc(sizeof(MxArrayList));
	
	if (result)
	{
		if (MxArrayListInit(result) != MxStatusOK)
		{
			free(result);
			result = NULL;
		}
	}
	
	return result;
}


MxArrayListRef MxArrayListCreateWithCapacity(size_t capacity)
{
	MxArrayListRef result = malloc(sizeof(MxArrayList));
	if (result)
	{
		if (MxArrayListInitWithCapacity(result, capacity) != MxStatusOK)
		{
			free(result);
			result = NULL;
		}
	}
	
	return result;
}


MxArrayListRef	MxArrayListCreateWithFunctions(MxFreeFunction itemFree, MxEqualsFunction itemEquals)
{
	MxArrayListRef result = malloc(sizeof(MxArrayList));
	
	if (result)
	{
		if (MxArrayListInitWithCapacityAndFunctions(result, MxArrayListDefaultCapacity, itemFree, itemEquals) != MxStatusOK)
		{
			free(result);
			result = NULL;
		}
	}
	
	return result;
}

MxArrayListRef MxArrayListCreateWithCapacityAndFunctions(size_t capacity, MxFreeFunction itemFree, MxEqualsFunction itemEquals){
	MxArrayListRef result = malloc(sizeof(MxArrayList));
	if (result)
	{
		if (MxArrayListInitWithCapacityAndFunctions(result, capacity, itemFree, itemEquals) != MxStatusOK)
		{
			free(result);
			result = NULL;
		}
	}
	
	return result;
}


MxStatus MxArrayListInit(MxArrayListRef list)
{
	return MxArrayListInitWithCapacityAndFunctions(list, 
                                                   MxArrayListDefaultCapacity,
                                                   NULL,
                                                   NULL);
}

MxStatus MxArrayListInitWithFunctions(MxArrayListRef list, MxFreeFunction itemFree, MxEqualsFunction itemEquals)
{
	return MxArrayListInitWithCapacityAndFunctions(list,
                                                   MxArrayListDefaultCapacity,
                                                   itemFree,
                                                   itemEquals);
}

MxStatus MxArrayListInitWithCapacity(MxArrayListRef	list, size_t capacity)
{
	return MxArrayListInitWithCapacityAndFunctions(list, capacity, NULL, NULL);
}

MxStatus MxArrayListInitWithCapacityAndFunctions(MxArrayListRef list, size_t capacity, MxFreeFunction itemFree, MxEqualsFunction itemEquals)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	if (capacity == 0)
		return MxStatusIllegalArgument;
	
	
    
	list->items = (void **)calloc(capacity, sizeof(void *));
	if (list->items == NULL)
		return MxStatusNoMemory;
	
	list->capacity = capacity;
	list->count = 0;
	list->itemEquals = itemEquals;
	list->itemFree = itemFree;
	
	return MxStatusOK;
}


MxStatus MxArrayListWipe(MxArrayListRef list)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxArrayListClear(list);
	if (result == MxStatusOK)
		free(list->items);
	
	return MxStatusOK;
}


MxStatus MxArrayListDelete(MxArrayListRef list)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxArrayListWipe(list);
	if (result == MxStatusOK)
		free(list);
	
	return result;
}

MxStatus MxArrayListAppend(MxArrayListRef list, const void *item)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = ExpandIfNeeded(list);
	if (result != MxStatusOK)
		return result;
	
	
	list->items[list->count] = (void *)item;
	list->count += 1;
	
	return MxStatusOK;
}



MxStatus MxArrayListPush(MxArrayListRef list, const void *item)
{
	if (list	 == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = ExpandIfNeeded(list);
	if (result != MxStatusOK)
		return result;
	
	memmove(list->items + 1, list->items, (sizeof(void *) * list->count));
	
	list->items[0] = (void *)item;
	
	return MxStatusOK;
}



MxStatus MxArrayListInsertAt(MxArrayListRef list, const void *item, int index)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	if (index < 0)
		return MxStatusIllegalArgument;
	
	if (index >= list->count)
		return MxStatusIndexOutOfRange;
	
	
  MxStatus result = ExpandIfNeeded(list);
  if (result != MxStatusOK)
    return result;
  
	void *moveSource = list->items + index;
	void *moveDest = list->items + (index + 1);
	size_t moveAmt = (list->count - index) * sizeof(void *);
	
	memmove(moveDest, moveSource, moveAmt);
	
	list->items[index] = (void *)item;
	list->count += 1;
	
	return MxStatusOK;
}



MxStatus MxArrayListReplaceAt(MxArrayListRef list, const void *item, int index)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	if (index < 0)
		return MxStatusIllegalArgument;
	
	if (index >= list->count)
		return MxStatusIndexOutOfRange;
	
	if (list	->itemFree)
		list->itemFree(list->items[index]);
	
	list->items[index] = (void *)item;
	
	// count remains the same...
	
	return MxStatusOK;
}



MxStatus MxArrayListItemAt(MxArrayListRef list, int index, void **result)
{
	if (list == NULL || result == NULL)
		return MxStatusNullArgument;
	
	if (index < 0)
		return MxStatusIllegalArgument;
	
	if (index >= list->count)
		return MxStatusIndexOutOfRange;
	
	*result = list->items[index];
	
	return MxStatusOK;
}



MxStatus MxArrayListPop(MxArrayListRef list, void **result)
{
	if (list == NULL || result == NULL)
		return MxStatusNullArgument;
	
	*result = NULL;
	if (list->count > 0)
	{
		*result = list->items[list->count - 1];
		list->items[list->count - 1] = NULL;
		list->count -= 1;
		
		// Dont't free the item - transfers responsibility for memory management
	}
	
	return MxStatusOK;
}



MxStatus MxArrayListRemoveAt(MxArrayListRef list, int index, void **removed)
{
	if (list	 == NULL || removed == NULL)
		return MxStatusNullArgument;
	
	if (index < 0)
		return MxStatusIllegalArgument;
	
	if (index >= list->count)
		return MxStatusIndexOutOfRange;
	
	
	*removed = list->items[index];
	
	void *moveSource = list->items + (index + 1);
	void *moveDest = list->items + index;
	size_t moveAmt = (list->count + (index + 1)) * sizeof(void *);
	
	
	memmove(moveDest, moveSource, moveAmt);
	
	list->items[list->count - 1] = NULL;
	
	list->count -= 1;
	
	return MxStatusOK;
}



MxStatus MxArrayListClear(MxArrayListRef list)
{
	if (list == NULL)
		return MxStatusNullArgument;
	
	if (list->itemFree)
	{
		for (int ctr = 0; ctr < list->count; ++ctr)
		{
			list->itemFree(list->items[ctr]);
		}
	}
	
	
	memset(list->items, 0, (list->count * sizeof(void *)));
	
	list->count = 0;
	
	return MxStatusOK;
}



MxStatus MxArrayListIterate(MxArrayListRef list, MxIteratorCallback callback, void *state)
{
	if (list == NULL || callback == NULL)
		return MxStatusNullArgument;
	
    MxStatus result = MxStatusOK;
	for (int ctr = 0; ctr < list->count; ++ctr)
		if ((result = callback(list->items[ctr], state)) != MxStatusOK)
			break;
	
	return result;
}



MxStatus MxArrayListIterateBackward(MxArrayListRef list, MxIteratorCallback callback, void *state)
{
	if (list == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxStatusOK;
    size_t ctr = list->count - 1;
    do {
        if ((result = callback(list->items[ctr], state)) != MxStatusOK)
            break;
    } while (ctr > 0);

	
	return result;
}



inline void *MxArrayListGetItem(MxArrayListRef list, int idx)
{
	if (list && idx > 0 && idx < list->count)
		return list->items[idx];
    
	return NULL;
}



inline size_t MxArrayListGetCount(MxArrayListRef list)
{
	if (list != NULL)
		return list->count;
	
	return 0;
}
