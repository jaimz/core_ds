//
//  test_bintree.c
//  core_ds
//
//  Created by J O'Brien on 12/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "MxStatus.h"
#include "MxBinaryTree.h"

#include "utils.h"
#include "test_bintree.h"



static int StringPrintingIterator(const void *item, void *state);
static int NumberPrintingIterator(const void *item, void *state);
static void PrintNumberTree(const char *title, MxBinaryTreeRef tree);
static void PrintStringTree(const char *title, MxBinaryTreeRef tree);
static void ShuffleIntArray(int array[], int len);


void test_bintree(void)
{
	char *data[] = {
		"bbb", "aaa",  "ddd", "ccc", "fff", "eee", 
		"jjj", "ggg", "hhh", "iii",  "kkk", "zzz",
		"ppp", "qqq", "rrr", "nnn", "www", "q", "w",
		"e", "r", "t", "y", "u", "i", "o", "p", "a",
		"a", "s", "d", "f", "g", "h", "j", "k", "l",
		"z", "x", "c", "v", "b", "n", "m", NULL
	};
    
	
	int numData[60] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 
		44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
		58, 59, 60
	};
    
	

	MxBinaryTree stringTree;
    MxBinaryTree numberTree;
	
	ShuffleIntArray(numData, 60);
    
    for (int i = 0; i < 60; ++i) {
        printf("%d  ", numData[i]);
    }
    printf("\n\n");
	
	
	MxStatus strStatus = MxBinaryTreeInitWithComparer(&stringTree, MxDefaultCStrCompareFunction);
   	MxStatus numStatus = MxBinaryTreeInitWithComparer(&numberTree, MxDefaultCompareIntFunction);
    
	if (strStatus != MxStatusOK)
		dieWithStatus("Init'ing string tree: ", strStatus);
    
    if (numStatus != MxStatusOK)
        dieWithStatus("Init'ing number tree: ", numStatus);
	
    
//	printf("Init'd OK\n"); fflush(stdout);
	
    
	int idx = 0;
	while (data[idx])
	{
        //		printf("Inserted %d\n", idx); fflush(stdout);
		strStatus = MxBinaryTreeInsert(&stringTree, data[idx]);
		if (strStatus != MxStatusOK)
			dieWithStatus("Inserting into string tree", strStatus);
		idx++;
	}
	
	idx = 0;
    while (idx < 60)
    {
        numStatus = MxBinaryTreeInsert(&numberTree, &(numData[idx]));
        if (numStatus != MxStatusOK)
            dieWithStatus("Inserting into number tree", numStatus);
        idx++;		
    }
	
	PrintStringTree("String tree", &stringTree);
    PrintNumberTree("Number tree", &numberTree);
    

	printf("String tree depth: %d\n", MxBinaryTreeDepth(&stringTree));
	printf("Number tree depth: %d\n", MxBinaryTreeDepth(&numberTree));

    
	if ((strStatus = MxBinaryTreeWipe(&stringTree)) != MxStatusOK) {
		dieWithStatus("Wiping string tree ", strStatus);
	}
    
    if ((numStatus = MxBinaryTreeWipe(&numberTree)) != MxStatusOK) {
        dieWithStatus("Wiping number tree", numStatus);
    }
}


static int StringPrintingIterator(const void *item, void *state)
{
	printf("%s\n", (const char *)item);
	return 0;
}

static int NumberPrintingIterator(const void *item, void *state)
{
    printf("%d\n", *((int *)item));
    return 0;
}


static void PrintStringTree(const char *title, MxBinaryTreeRef tree)
{
    printf("--- %s\n", title);
	MxStatus s = MxStatusOK;
	if ((s = MxBinaryTreeWalkInOrder(tree, StringPrintingIterator, NULL)) != MxStatusOK)
		dieWithStatus("Printing string tree", s);
}

static void PrintNumberTree(const char *title, MxBinaryTreeRef tree)
{
    printf("---Number: %s (%d items)\n", title, MxBinaryTreeCount(tree));
	MxStatus s = MxStatusOK;
	if ((s = MxBinaryTreeWalkInOrder(tree, NumberPrintingIterator, NULL)) != MxStatusOK)
		dieWithStatus("Printing number tree", s);
}


// Randomise an int array...
static void ShuffleIntArray(int array[], int len)
{
    srandom(getpid());
	int ctr = len / 2;
	int idx1 = 0;
	int idx2 = 0;
	int tmp;
	while (ctr > 0)
	{
		idx1 = (int)(random() % len);
		idx2 = (int)(random() % len);
		
		tmp = array[idx1];
		array[idx1] = array[idx2];
		array[idx2] = tmp;
		
		ctr--;
	}
}