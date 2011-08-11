//
//  test_list.c
//  core_ds
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "utils.h"
#include "test_list.h"

#include <stdio.h>
#include <stdlib.h>

#include "MxStatus.h"
#include "MxList.h"

static int PrintStringIterator(const void *string, void *state);
static void PrintList(const char *title, MxListRef list);
static int FilterCallback(const void *entry, void *state);
static int MapCallback(const void *entry, void *state);


void test_list(void)
{
	static char* listItems[] = {
		"First item",
		"Second item",
		"Third item",
		"Fourth item",
		"Fifth item",
		NULL
	};
	
	static char* pushedItems[] = {
		"+ Pushed 1",
		"+ Pushed 2",
		NULL
	};
    
    
	MxListRef list = MxListCreateWithFunctions(NULL, NULL);
	if (!list)
		die("Could not create list - probably out of memory.");
	
	int itemIdx = 0;
	MxStatus status = MxStatusOK;
	while (listItems[itemIdx])
	{
		if ((status = MxListAppend(list, listItems[itemIdx])) != MxStatusOK)
			dieWithStatus("Problem appending to list", status);
		itemIdx++;
	}
	
	PrintList("Initial", list);
	
	itemIdx = 0;
	while(pushedItems[itemIdx])
	{
		if ((status = MxListPush(list, pushedItems[itemIdx])) != MxStatusOK)
			dieWithStatus("Problem pushing ", status);
		itemIdx++;
	}
	
	PrintList("After push", list);
	
	if ((status = MxListInsertAfter(list, listItems[2], "* After third")) != MxStatusOK)
		dieWithStatus("Attempting to insert", status);
	
	PrintList("Insert after item", list);
	
	if ((status = MxListInsertAtIndex(list, 1, "* Inserted at 1")) != MxStatusOK)
		dieWithStatus("Attempting to insert at index", status);
	
	PrintList("Insert at index", list);
	
	if ((status = MxListInsertAtIndex(list, 1, "* Inserted at 1")) != MxStatusOK)
		dieWithStatus("Second index insert", status);
	
	PrintList("Insert at index (again)", list);
	
	char *result = NULL;
	if ((status = MxListPop(list, (void **)&result)) != MxStatusOK)
		dieWithStatus("Popping value", status);
	
	printf("Popped off: %s\n", result);
	
	if ((status = MxListPop(list, (void **)&result)) != MxStatusOK)
		dieWithStatus("Second pop", status);
	
	printf("Popped off: %s\n", result);
	
	PrintList("After pops", list);
	
	if ((status = MxListDequeue(list, (void **)&result)) != MxStatusOK)
		dieWithStatus("Dequeue", status);
	
	printf("Dequeued %s\n", result);
	
	if ((status = MxListDequeue(list, (void **)&result)) != MxStatusOK)
		dieWithStatus("Second dequeue", status);
	
	printf("Dequeued %s\n", result);
	
	PrintList("After dequeue", list);
	
	printf("\nFiltering '+'s...\n");
	
	if ((status = MxListFilter(list, FilterCallback, NULL)) != MxStatusOK)
		dieWithStatus("Filtering list", status);
	
	PrintList("After filtering", list);
	
	
	printf("\nMapping...\n");
	MxList mappedList;
	if ((status = MxListInitWithFunctions(&mappedList, NULL, NULL)) != MxStatusOK)
		dieWithStatus("Initing list on stack", status);
    
	if ((status = MxListMap(list, MapCallback, NULL, &mappedList)) != MxStatusOK)
		dieWithStatus("Mapping list", status);
    
	PrintList("The mapped list", &mappedList);
	
	if ((status = MxListWipe(&mappedList)) != MxStatusOK)
		dieWithStatus("Wiping mapped list", status);
	
	
	if ((status = MxListInsertAfter(list, listItems[1], "+ Inserted after second")) != MxStatusOK)
		dieWithStatus("Insert after", status);
	
	if ((status = MxListInsertBefore(list, listItems[1], "+ Inserted before second")) != MxStatusOK)
		dieWithStatus("Insert before", status);
	
	PrintList("After wipe & inserts", list);
    
    if ((status = MxListClear(list)) != MxStatusOK)
        dieWithStatus("Clearing", status);
	
    PrintList("After wipe", list);
    
	if ((status = MxListDelete(list)) != MxStatusOK)
		dieWithStatus("Deleting list", status);
}


static MxStatus PrintStringIterator(const void *string, void *state)
{
	printf("%s\n", (const char *)string);
	return MxStatusOK;
}

static int FilterCallback(const void *entry, void *state)
{
	return ((const char *)entry)[0] == '+';
}

static int MapCallback(const void *entry, void *state)
{
	return ((const char *)entry)[0] == '*';
};


static void PrintList(const char *title, MxListRef list)
{
    printf("\n--- %s: (%d items) ----\n", title, MxListGetCount(list));

    
	MxStatus status = MxListIterateForward(list, PrintStringIterator, NULL);
    
	if (status != MxStatusOK)
		MxStatusError("Iterating list", status);
    
	printf("------------\n");
}