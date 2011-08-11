//
//  test_hashtable.c
//  core_ds
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>

#include "utils.h"
#include "test_hashtable.h"

#include "MxList.h"
#include "MxHashtable.h"

static void PrintTable(const char *title, MxHashtableRef table);
static MxStatus PrintPair(const void *key, const void *value, void *state);
static int PrintCallback(const void *value, void *state);

void test_hashtable(void)
{
	static char *tableContents[] = {
		"FirstKey", "FirstValue",
		"SecondKey", "SecondValue",
		"ThirdKey", "ThirdValue",
		"FourthKey", "FourthValue",
		NULL
	};
	
	MxHashtableRef table = MxHashtableCreatePropertyMap();
	if (!table)
		die("Couldn't create table - probably no memory");
	
	MxStatus status = MxStatusOK;
	
	if ((status = MxHashtableSetKeyFreeFunction(table, NULL)) != MxStatusOK)
		dieWithStatus("Setting key free", status);
	
	if ((status = MxHashtableSetValueFreeFunction(table, NULL)) != MxStatusOK)
		dieWithStatus("Setting value free", status);
	
	
	int idx = 0;
	while (tableContents[idx]) {
		if ((status = MxHashtablePut(table, tableContents[idx], tableContents[idx+1])) != MxStatusOK)
			dieWithStatus("put", status);
		idx += 2;
	}
	
	PrintTable("Initial", table);
	
	char *result = NULL;
	if ((status = MxHashtableGet(table, "SecondKey", (void **)&result)) != MxStatusOK)
		dieWithStatus("Table get", status);
    
	printf("Value for \"SecondKey\" : %s\n", result);
	
	PrintTable("After get 'SecondKey'", table);
	
	if ((status = MxHashtableTake(table, "ThirdKey", (void **)&result)) != MxStatusOK)
		dieWithStatus("Table take", status);
	
	printf("Value from take(\"ThirdKey\"): %s\n", result);
	
	PrintTable("After take 'ThirdKey'", table);
	
	printf("Keys: ");
	if ((status = MxHashtableIterateKeys(table, PrintCallback, NULL)) != MxStatusOK)
		dieWithStatus("Iterating keys", status);
	printf("\n\n");
	
	printf("Values: ");
	if ((status = MxHashtableIterateValues(table, PrintCallback, NULL)) != MxStatusOK)
		dieWithStatus("Iterating values", status);
	printf("\n\n");
	
	if ((status = MxHashtableRemove(table, "FourthKey")) != MxStatusOK)
		dieWithStatus("removing", status);
	
	PrintTable("Removed 'FourthKey'", table);
	
	status = MxHashtableRemove(table, "DoesntExist");
	if (status == MxStatusNotFound)
		printf("DoesntExist not found\n");
	else
		printf("Unexpected status: %s\n", MxStatusMsg(status));
	
	if ((status = MxHashtablePut(table, "SecondKey", "NewSecondValue")) != MxStatusOK)
		dieWithStatus("Replacing value", status);
	
	PrintTable("Replaced 'SecondKey'", table);
    
	printf("Clearing..."); fflush(stdout);
	if ((status = MxHashtableClear(table)) != MxStatusOK)
		dieWithStatus("Clearing", status);
	printf("Done."); fflush(stdout);
	
	PrintTable("After clear", table);
	
	if ((status	= MxHashtablePut(table, "foo", "bar")) != MxStatusOK)
		dieWithStatus("putting after clear", status);
	
	PrintTable("Put following clear", table);
	
	if ((status	= MxHashtablePut(table, "boo", "baz")) != MxStatusOK)
		dieWithStatus("putting after clear", status);
	
	PrintTable("Another put", table);
	
	status = MxHashtableContainsKey(table, "foo");
	printf("Contains \"foo\": (%d) %s\n", status, MxStatusMsg(status));
	
	MxHashtableDelete(table);
}

static MxStatus PrintCallback(const void *data, void *state)
{
	printf("%s ", (const char *)data);
	return MxStatusOK;
}


static MxStatus PrintPair(const void *key, const void *value, void *state)
{
	printf("%s => %s\n", (const char *)key, (const char *)value);
	return MxStatusOK;
}


static void PrintTable(const char *title, MxHashtableRef table)
{
	printf("\n-- %s ------\n", title);
	MxHashtableIteratePairs(table, PrintPair, NULL);
	printf("---------------\n");
	printf("Bucket counts (%d items): ", MxHashtableGetCount(table));
	for (unsigned int ctr = 0; ctr < table->bucketCount; ctr++)
	{
		printf("%d ", MxListGetCount((MxListRef)(table->buckets+ctr)));
	}
	printf("\n---------------\n");
}