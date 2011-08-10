//
//  MxBinaryTree.h
//  core_ds
//
//  Simple binary tree. No balancing.
//
//  Created by J O'Brien on 10/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxBinaryTree_h
#define core_ds_MxBinaryTree_h

#include "MxStatus.h"
#include "MxFunctions.h"

typedef struct _MxBinaryTreeNode {
	void *data;
	struct _MxBinaryTreeNode *parent;
	struct _MxBinaryTreeNode *left;
	struct _MxBinaryTreeNode *right;
} MxBinaryTreeNode, *MxBinaryTreeNodeRef;


typedef struct _MxBinaryTree {
	MxBinaryTreeNodeRef root;
	int count;
	
	MxFreeFunction itemFree;
	MxCompareFunction itemCompare;
} MxBinaryTree, *MxBinaryTreeRef;


MxBinaryTreeRef MxBinaryTreeCreate(void);
MxStatus MxBinaryTreeInit(MxBinaryTreeRef tree);
MxStatus MxBinaryTreeInitWithComparer(MxBinaryTreeRef tree, MxCompareFunction comparer);

MxStatus MxBinaryTreeClear(MxBinaryTreeRef tree);

MxStatus MxBinaryTreeInsert(MxBinaryTreeRef tree, void *item);
MxStatus MxBinaryTreeRemove(MxBinaryTreeRef tree, void *item, void **oldItem);
MxStatus MxBinaryTreeFind(MxBinaryTreeRef tree, void *item, void **resultItem);

int MxBinaryTreeDepth(MxBinaryTreeRef tree);

MxStatus MxBinaryTreeWalkInOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state);
MxStatus MxBinaryTreeWalkPreOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state);
MxStatus MxBinaryTreeWalkPostOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state);


MxStatus MxBinaryTreeWipe(MxBinaryTreeRef tree);
MxStatus MxBinaryTreeDestroy(MxBinaryTreeRef tree);

#endif
