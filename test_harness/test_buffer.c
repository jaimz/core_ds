//
//  test_buffer.c
//  core_ds
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>

#include "utils.h"
#include "test_buffer.h"

#include "MxStatus.h"
#include "MxStringBuffer.h"


static int PrintStructInfo = 1;

static void PrintVisRep(MxStringBufferRef buffer);
static void PrintBuffer(MxStringBufferRef buffer);


void test_buffer()
{
	MxStringBuffer buf = {0};
	
	MxStatus status = MxStatusOK;
	
	printf(" -- String buffer ----------\n");
	printf("Initialising buffer...\n");
	
	if ((status = MxStringBufferInitWithCharacters(&buf, "Original contents", 17)) != MxStatusOK)
		dieWithStatus("Initialising", status);
	
	PrintBuffer(&buf);
	
	if ((status = MxStringBufferInsertAtPoint(&buf, "Prefix: ", 0)) != MxStatusOK)
		dieWithStatus("Insert", status);
	
	PrintBuffer(&buf);
	
	if ((status = MxStringBufferInsertInt(&buf, 200)) != MxStatusOK)
		dieWithStatus("Insert int", status);
	
	if ((status = MxStringBufferInsert(&buf, " ")) != MxStatusOK)
		dieWithStatus("Inserting space", status);
	
	if ((status = MxStringBufferInsertInt(&buf, -200)) != MxStatusOK)
		dieWithStatus("Insert int 2", status);
    
	if ((status = MxStringBufferInsert(&buf, " ")) != MxStatusOK)
		dieWithStatus("Inserting space 2", status);
	
	if ((status = MxStringBufferInsertInt(&buf, 0)) != MxStatusOK)
		dieWithStatus("Insert int 3", status);
	
	if ((status = MxStringBufferInsert(&buf, " ")) != MxStatusOK)
		dieWithStatus("Inserting space 3", status);
	
	
	PrintBuffer(&buf);
	
	printf("\n\nAs cstr: %s\n", MxStringBufferAsCStr(&buf));
	
	if ((status = MxStringBufferWipe(&buf)) != MxStatusOK)
		dieWithStatus("Wiping", status);
}

static void PrintBuffer(MxStringBufferRef buffer)
{
	PrintVisRep(buffer);
	
	if (PrintStructInfo)
	{
		printf(
			   "{\n\tgs: %lu\n\tge: %lu\n\tpoint: %lu\n\tcount: %lu\n\tbuflen: %lu\n\tcapacity: %lu\n}\n\n",
			   buffer->gs, buffer->ge, buffer->point, buffer->count, buffer->bufLen, buffer->capacity
			   );
	}	
}

static void PrintVisRep(MxStringBufferRef buffer)
{
	int ctr = 0;
	
	printf("\n\n");
	
	while (ctr < buffer->gs)
	{
		printf("%c", buffer->storage[ctr]);
		ctr++;
	}
	
	while (ctr < buffer->ge)
	{
		printf("_");
		ctr++;
	}
	
	while (ctr < buffer->bufLen)
	{
		printf("%c", buffer->storage[ctr]);
		ctr++;
	}
	
	printf("\n\n");
}