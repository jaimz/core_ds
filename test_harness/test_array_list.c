//
//  test_array_list.c
//  core_ds
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "test_array_list.h"

#include <stdio.h>
#include <stdlib.h>

#include "MxArrayList.h"

#include "utils.h"

#define check(preamble, call) { status = call; dieIfBad(preamble, status);}

static int PrintingIterator(const void *item, void *state);
static void PrintArrayList(MxArrayListRef list);


void test_array_list(void)
{
	static char *listItems[] = {
		"First item",
		"Second item",
		"Third item",
		"Fourth item",
		"Fifth item",
		NULL
	};
	
	static char *PushedItems[] = {
		"+ Pushed 1",
		"+ Pushed 2",
		NULL
	};
	
	static char *InsertedItems[] = {
		"* Inserted 1",
		"* Inserted 2",
		NULL
	};
	
	static char *Replacements[] = {
		"/ Replacement 1",
	};
	
	
	MxArrayList list;
	MxStatus status = MxArrayListInitWithCapacity(&list, 3);
	dieIfBad("Initialising...", status);
	
	printf("Appending...\n");
	int idx = 0;
	while (listItems[idx])
	{
		status = MxArrayListAppend(&list, listItems[idx]);
		dieIfBad("Appending", status);
		idx += 1;
	}
	
	PrintArrayList(&list);
	
	
	printf("Inserting...\n");
	status = MxArrayListInsertAt(&list, InsertedItems[0], 1);
	dieIfBad("First insert", status);
	
    
	status = MxArrayListInsertAt(&list, InsertedItems[1], 3);
	dieIfBad("Second insert", status);
	
	PrintArrayList(&list);
	
    
	printf("Pushing...\n");
	check("First push", MxArrayListPush(&list, PushedItems[0]));
	
	check("Second push", MxArrayListPush(&list, PushedItems[1]));
	
	PrintArrayList(&list);
	
	printf("Replacing...\n");
	check("Replacement", MxArrayListReplaceAt(&list, Replacements[0], 2));
	
	PrintArrayList(&list);
	
    
	char *popped;
	printf("Popping...\n");
	check("First pop", MxArrayListPop(&list, (void **)&popped));
	if (popped) 
		printf("Popped: %s\n", popped);
	else
		printf("Popped was NULL\n");
    
    
	check("Second pop", MxArrayListPop(&list, (void **)&popped));
	printf("Popped: %s\n", popped);
	
	PrintArrayList(&list);
	
	printf("Removing...\n");
	check("Removing", MxArrayListRemoveAt(&list, 3, (void **)&popped));
	printf("Removal result: %s\n", popped);
	
	PrintArrayList(&list);
	
	printf("Item at...\n");
	check("Item at", MxArrayListItemAt(&list, 3, (void **)&popped));
	printf("Item at 3: %s\n", popped);
	
	printf("Clearing...\n");
	check("Clearing", MxArrayListClear(&list));
	PrintArrayList(&list);
	
	printf("New appends...\n");
	idx = 0;
	while (InsertedItems[idx])
	{
		check("Inserting", MxArrayListAppend(&list, InsertedItems[idx++]));
	}
	
	PrintArrayList(&list);
}

static int PrintCallback(const void *value, void *state)
{
	fprintf(stdout, "%s\n", (const char *)value);
	return 0;
}

static void PrintArrayList(MxArrayListRef list)
{
	printf("-- list (%lu items) ------\n", list->count);
	MxStatus status = MxStatusOK;

	check("Printing", MxArrayListIterate(list, PrintCallback,  NULL));
	printf("-------------------------\n");
}