//
//  MxHeap.c
//  core_ds
//
//  Created by J O'Brien on 25/11/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "MxHeap.h"
#include "MxStatus.h"
#include "MxArrayList.h"

inline MxStatus MxHeapInit(MxHeapRef heap)
{
    return MxHeapInitWithCapacityAndFunctions(heap, MxHeapDefaultCapacity, NULL, MxDefaultCompareFunction);
}

inline MxStatus MxHeapInitWithCapacity(MxHeapRef heap, size_t capacity)
{
    return MxHeapInitWithCapacityAndFunctions(heap, capacity, NULL, MxDefaultCompareFunction);
}

inline MxStatus MxHeapInitWithFunctions(MxHeapRef heap, MxFreeFunction itemFree, MxCompareFunction itemCompare)
{
    return MxHeapInitWithCapacityAndFunctions(heap, MxHeapDefaultCapacity, itemFree, itemCompare);
}


MxStatus MxHeapInitWithCapacityAndFunctions(MxHeapRef heap, size_t capacity, MxFreeFunction itemFree, MxCompareFunction itemCompare)
{
    if (heap == NULL)
        return MxStatusNullArgument;
    
    MxStatus status = MxHeapWipe(heap);
    if (status != MxStatusOK)
        return status;
    
    heap->_content = calloc(capacity, sizeof(void *));
    

    //heap->_content = MxArrayListCreateWithCapacityAndFunctions(capacity, itemFree, itemFree);
    
    heap->_item_free = itemFree;
    heap->_compare_fun = itemCompare;


    return MxStatusOK;
}


inline MxHeapRef MxHeapCreate()
{
    return MxHeapCreateWithCapacityAndFunctions(MxHeapDefaultCapacity, NULL, MxDefaultCompareFunction);
}

inline MxHeapRef MxHeapCreateWithCapacity(size_t capacity)
{
    return MxHeapCreateWithCapacityAndFunctions(capacity, NULL, MxDefaultCompareFunction);
}

inline MxHeapRef MxHeapCreateWithFunctions(MxFreeFunction itemFree, MxCompareFunction itemCompare)
{
    return MxHeapCreateWithCapacityAndFunctions(MxHeapDefaultCapacity, itemFree, itemCompare);
}

MxHeapRef MxHeapCreateWithCapacityAndFunctions(size_t capacity, MxFreeFunction itemFree, MxCompareFunction itemCompare)
{
    MxHeapRef result = malloc(sizeof(MxHeap));
    if (result != NULL)
    {
        MxStatus status = MxHeapInitWithCapacityAndFunctions(result, capacity, itemFree, itemCompare);
        if (status != MxStatusOK)
        {
            free(result);
            result = NULL;
        }
    }
    
    return result;
}


MxStatus MxHeapAdd(MxHeapRef heap, void *item);
MxStatus MxHeapRemove(MxHeapRef heap, void *item);
MxStatus MxHeapGetTopmost(MxHeapRef heap, void **result)
{
    if (heap == NULL || result == NULL)
        return MxStatusNullArgument;
    
    return MxArrayListItemAt(heap->_content, 0, result);
}

MxStatus MxHeapGetBottommost(MxHeapRef heap, void **result);


MxStatus MxHeapWipe(MxHeapRef heap)
{
    return MxStatusNotImplemented;
}

MxStatus MxHeapDelete(MxHeapRef heap)
{
    if (heap == NULL)
        return MxStatusNullArgument;
    
    MxStatus status = MxHeapWipe(heap);
    if (status == MxStatusOK)
    {
        free(heap);
    }
    
    return status;
}
