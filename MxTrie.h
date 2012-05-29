//
//  MxTrie.h
//  core_ds
//
//  Created by J O'Brien on 03/01/2012.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxTrie_h
#define core_ds_MxTrie_h

#include "MxStatus.h"
#include "MxArrayList.h"
#include "MxFunctions.h"


typedef struct _MxTrieNode {
    void *content;
    int count;
    MxArrayListRef _children;
} MxTrieNode, *MxTreeNodeRef;


typedef struct _MxTrie {
    MxTreeNodeRef _root;
    int count;
    
    MxFreeFunction _nodeFree;
} MxTrie, *MxTrieRef;

#endif
