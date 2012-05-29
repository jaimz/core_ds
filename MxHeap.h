//
//  MxHeap.h
//  core_ds
//
//  Created by J O'Brien on 22/11/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxHeap_h
#define core_ds_MxHeap_h

#include "MxStatus.h"
#include "MxArrayList.h"
#include "MxFunctions.h"

#define MxHeapDefaultCapacity (128)

typedef struct _MxHeap {
    MxArrayListRef _content;
    MxCompareFunction _compare_fun;
    MxFreeFunction _item_free;
} MxHeap, *MxHeapRef;


MxStatus MxHeapInit(MxHeapRef heap);
MxStatus MxHeapInitWithCapacity(MxHeapRef heap, size_t capacity);
MxStatus MxHeapInitWithFunctions(MxHeapRef heap, MxFreeFunction itemFree, MxCompareFunction itemCompare);
MxStatus MxHeapInitWithCapacityAndFunctions(MxHeapRef heap, size_t capacity, MxFreeFunction itemFree, MxCompareFunction itemCompare);


MxHeapRef MxHeapCreate();
MxHeapRef MxHeapCreateWithCapacity(size_t capacity);
MxHeapRef MxHeapCreateWithFunctions(MxFreeFunction itemFree, MxCompareFunction itemCompare);
MxHeapRef MxHeapCreateWithCapacityAndFunctions(size_t capacity, MxFreeFunction itemFree, MxCompareFunction itemCompare);


MxStatus MxHeapAdd(MxHeapRef heap, void *item);
MxStatus MxHeapRemove(MxHeapRef heap, void *item);
MxStatus MxHeapGetTopmost(MxHeapRef heap, void **result);
MxStatus MxHeapGetBottommost(MxHeapRef heap, void **result);


MxStatus MxHeapWipe(MxHeapRef heap);
MxStatus MxHeapDelete(MxHeapRef heap);


#endif
