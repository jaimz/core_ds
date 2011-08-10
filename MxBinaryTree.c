//
//  MxBinaryTree.c
//  core_ds
//
//  Created by J O'Brien on 10/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "MxDebug.h"
#include "MxBinaryTree.h"
#include "MxFunctions.h"

static MxBinaryTreeNodeRef CreateNode(void *data);
static MxBinaryTreeNodeRef FindParentFor(MxBinaryTreeNodeRef node, void *data, MxCompareFunction compare);
static MxBinaryTreeNodeRef FindNodeFor(MxBinaryTreeNodeRef root, void *data, MxCompareFunction compare);
static MxStatus FindInsertionPointFor(MxBinaryTreeNodeRef root, void *data, MxCompareFunction compare, MxBinaryTreeNodeRef *result);
static MxStatus WalkNodeInOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state);
static MxStatus WalkNodePreOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state);
static MxStatus WalkNodePostOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state);

static void WipeNode(MxBinaryTreeNodeRef);


static MxBinaryTreeNodeRef CreateNode(void *data)
{
	MxBinaryTreeNodeRef node = (MxBinaryTreeNodeRef)malloc(sizeof(MxBinaryTreeNode));
	
	if (node)
	{
		node->data = data;
		node->left = NULL;
		node->right = NULL;
	}
	
	return node;
}

static MxBinaryTreeNodeRef FindParentFor(MxBinaryTreeNodeRef root, void *data, MxCompareFunction compare)
{
	if (root == NULL || compare == NULL || (compare(root->data, data) == 0))
		return root;
    
	MxBinaryTreeNodeRef curr = root;
	MxBinaryTreeNodeRef result = NULL;
	
	
	int cmp;	
	while (curr)
	{
		cmp = compare(curr->data, data);
		if (cmp < 0)
		{
			if (curr->left)
			{
				if (compare(curr->left->data, data) == 0)
				{
					result = curr;
					break;
				}
				else
				{
					curr = curr->left;
				}
			}
		}
		else if (cmp > 0)
		{
			if (curr->right)
			{
				if (compare(curr->right->data, data) == 0)
				{
					result = curr;
					break;
				}
				else
				{
					curr = curr->right;
				}
			}
		}
		else
		{
			// Only happens when data is the value in root - in which case we will return NULL...
			MxDebug("Asked for a parent of the root value in binary tree.");
			curr = NULL;
		}
	}
	
    
	return result;
}

static MxStatus FindInsertionPointFor(MxBinaryTreeNodeRef root, void *data, MxCompareFunction compare, MxBinaryTreeNodeRef *result)
{
	if (root == NULL || compare == NULL)
		return MxStatusNotFound;
	
	MxBinaryTreeNodeRef curr = root;
	MxStatus status = MxStatusIncomplete;
	*result = NULL;
	
	int cmp;
	while (status == MxStatusIncomplete)
	{
		cmp = compare(curr->data, data);
		MxDebug("Compare result (%s, %s): %d\n", (char *)data, (char *)curr->data, cmp);
		if (cmp == 0)
		{
			// The data is already in the tree - return the current node for the data
			// as the 'parent' node - the caller can decide whether to over
			status = MxStatusExists;
			*result = NULL;
		}
		else if (cmp < 0)
		{
			if (curr->right == NULL)
			{
				*result = curr;
				status = MxStatusOK;
			}
			else
			{
				curr = curr->right;
			}
		}
		else if (cmp > 0)
		{
			if (curr->left == NULL)
            {
                *result = curr;
                status = MxStatusOK;
            }
            else
            {
                curr = curr->left;
            }
		}
	}
	
	return status;
}

static MxBinaryTreeNodeRef FindNodeFor(MxBinaryTreeNodeRef root, void *data, MxCompareFunction compare)
{
	if (root == NULL || data == NULL || compare == NULL)
		return NULL;
	
	MxBinaryTreeNodeRef result = NULL;
	MxBinaryTreeNodeRef curr = root;
	
	int cmp = 0;
	while (curr) {
		cmp = compare(curr->data, data);
		if (cmp == 0)
		{
            result = curr;
            curr = NULL;
		}
		else if (cmp < 0)
		{
			curr = curr->left;
		}
		else if (cmp > 0)
		{
			curr = curr->right;
		}
	}
	
	return result;
}


MxBinaryTreeRef MxBinaryTreeCreate()
{
	MxBinaryTreeRef result = (MxBinaryTreeRef)malloc(sizeof(MxBinaryTree));
	if (result != NULL)
	{
		if (MxBinaryTreeInit(result) != MxStatusOK)
		{
			free(result);
			result = NULL;
		}
	}
	
	return result;
}

MxStatus MxBinaryTreeInit(MxBinaryTreeRef tree)
{
	return MxBinaryTreeInitWithComparer(tree, MxDefaultCompareFunction);
}

MxStatus MxBinaryTreeInitWithComparer(MxBinaryTreeRef tree, MxCompareFunction compare)
{
	if (tree == NULL || compare == NULL)
		return MxStatusNullArgument;
	
	tree->root = NULL;
	tree->count = 0;
	tree->itemCompare = compare;
    
	
	return MxStatusOK;
}

MxStatus MxBinaryTreeInsert(MxBinaryTreeRef tree, void *item)
{
	if (tree == NULL || item == NULL)
		return MxStatusNullArgument;
	
	if (tree->itemCompare == NULL)
		return MxStatusIllegalArgument;
	
	
	MxBinaryTreeNodeRef node = CreateNode(item);
	if (node == NULL)
		return MxStatusNullArgument;
	
	if (tree->root == NULL)
	{
		tree->root = node;
		return MxStatusOK;
	}
	
	
	MxBinaryTreeNodeRef parent;
	MxStatus status = FindInsertionPointFor(tree->root, item, tree->itemCompare, &parent);
	if (status != MxStatusOK)
	{
		if (status == MxStatusExists)
			return MxStatusOK;  // Item is already in the tree
		else
			return status;
	}
	
	if (parent == NULL)
	{
		MxDebug("Parent node was NULL searching for a place to put new item");
		return MxStatusUnknownError;
	}
	
	int cmp = tree->itemCompare(item, parent->data);
	if (cmp < 0)
		parent->left = node;
	else if (cmp > 0)
		parent->right = node;
	else
		; // item already in tree...
	
	
	return MxStatusOK;
}


MxStatus MxBinaryTreeRemove(MxBinaryTreeRef tree, void *item, void **oldItem)
{
	return MxStatusNotImplemented;
}


MxStatus MxBinaryTreeFind(MxBinaryTreeRef tree, void *item, void **resultItem)
{
	return MxStatusNotImplemented;
}


MxStatus MxBinaryTreeClear(MxBinaryTreeRef tree)
{
	return MxStatusNotImplemented;
}


MxStatus MxBinaryTreeWalkInOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state)
{
	if (tree == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	if (tree->root == NULL)
		return MxStatusOK;
	
	return WalkNodeInOrder(tree->root, callback, state);
}

MxStatus WalkNodeInOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state)
{
	if (node	 == NULL)
		return MxStatusOK;
	
	MxStatus status	= MxStatusOK;
	
	if ((status = WalkNodeInOrder(node->left, callback, state)) != MxStatusOK)
		return status;
	
	callback(node->data, state);
	
	if ((status = WalkNodeInOrder(node->right, callback, state)) != MxStatusOK)
		return status;
	
	return status;
}



MxStatus MxBinaryTreeWalkPreOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state)
{
	if (tree == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	if (tree->root == NULL)
		return MxStatusOK;
	
	return WalkNodePreOrder(tree->root, callback, state);
}

MxStatus WalkNodePreOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state)
{
	if (node == NULL)
		return MxStatusOK;
	
	MxStatus status = MxStatusOK;
	
	callback(node->data, state);
	
	if ((status = WalkNodePreOrder(node->left, callback, state)) != MxStatusOK)
		return status;
    
	if ((status	= WalkNodePreOrder(node->right, callback, state)) != MxStatusOK)
		return status;
	
	return status;
}



MxStatus MxBinaryTreeWalkPostOrder(MxBinaryTreeRef tree, MxIteratorCallback callback, void *state)
{
	if (tree == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	if (tree->root == NULL)
		return MxStatusOK;
	
	return WalkNodePostOrder(tree->root, callback, state);
}

MxStatus WalkNodePostOrder(MxBinaryTreeNodeRef node, MxIteratorCallback callback, void *state)
{
	if (node == NULL)
		return MxStatusOK;
	
	
	MxStatus status = MxStatusOK;
	
	if ((status = WalkNodePostOrder(node->left, callback, state)) != MxStatusOK)
		return status;
	
	if ((status	= WalkNodePostOrder(node->right, callback, state)) != MxStatusOK)
		return status;
    
	callback(node->data, state);
	
	return status;
}

static void CountDepth(MxBinaryTreeNodeRef node, int level, int *depthCount)
{
	if (level > *depthCount)
		*depthCount = level;
	
	if (node->left)
		CountDepth(node->left, level+1, depthCount);
	
	if (node->right)
		CountDepth(node->right, level+1, depthCount);
}


int MxBinaryTreeDepth(MxBinaryTreeRef tree)
{
	int result = 0;
	if (tree->root == NULL)
		return result;
	
	CountDepth(tree->root, 1, &result);
	
	return result;
}


static void WipeNode(MxBinaryTreeNodeRef node)
{
	if (node == NULL)
		return;
	
	WipeNode(node->left);
	WipeNode(node->right);
	free(node);
}

MxStatus MxBinaryTreeWipe(MxBinaryTreeRef tree)
{
	// Need to walk post-order, freeing parent nodes after their child nodes have been walked...
	
	if (tree == NULL)
		return MxStatusNullArgument;
	
	if (tree->root != NULL)
		WipeNode(tree->root);
    
	return MxStatusOK;
}

MxStatus MxBinaryTreeDestroy(MxBinaryTreeRef tree)
{
	if (tree == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxBinaryTreeWipe(tree);
	if (status == MxStatusOK)
		free(tree);
	
	return status;
}
