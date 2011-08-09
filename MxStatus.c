//
//  MxStatus.c
//  core_ds
//
//  Created by J O'Brien on 06/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/errno.h>

#include "MxStatus.h"

char *MxStatusStrings[] = {
	"False",
	"OK",
	"Out of memory",
	"Null argument",
	"Bad argument",
	"Index out of range",
	"Functionality not implemented",
	"Uknown error",
	"Illegal argument",
	"Read error",
	"End of file reached",
	"Could not open",
	"Invalid structure",
	"Write error",
	"Not found",
	"Items equal",
	"Item less",
	"Item greater",
	"Config value missing",
	"Unix error",
	"Filename too long",
	"Operation incomplete",
	"Item exists",
	"Iteration broken",
	NULL
};

static void out(FILE *f, const char *preamble, MxStatus status);


char *MxStatusMsg(MxStatus status)
{
	if (status == MxStatusTrue)
		return "True";
	
	static int msgCount = -1;
	char *result = NULL;
	
	if (msgCount == -1)
	{
		int tmpCount = 0;
		char *curr = MxStatusStrings[0];
		
		while (curr != NULL && tmpCount < MxStatusValueLimit)
		{
			tmpCount++;
			curr = MxStatusStrings[tmpCount];
		}
		
		if (tmpCount < MxStatusValueLimit)
			msgCount = tmpCount;
	}
	
	if (msgCount != -1)
	{
		int idx = 0 - status;
		if (idx < msgCount)
			result = MxStatusStrings[idx];
	}
	
	return result;
}



void MxStatusPrint(const char *preamble, MxStatus status)
{
	out(stdout, preamble, status);
}


void MxStatusError(const char *preamble, MxStatus status)
{
	out(stderr, preamble, status);
}

void MxStatusErrorWithStdErr(const char *preamble, MxStatus status)
{
	out(stderr, preamble, status);
	fprintf(stderr, "\tstd error: %s", strerror(errno));
}


static void out(FILE *f, const char *preamble, MxStatus status)
{
	char *msg = MxStatusMsg(status);
	if (msg != NULL)
	{
		fprintf(f, "%s: %s\n", 
                (preamble == NULL)?"":preamble, 
                msg); // ::I18N::
	}
	else
	{
		fprintf(f, 
                "%s: Could not find status message; value was %d\n", 
                preamble, status); // ::I18N::
	}
}