//
//  utils.c
//  tests
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(-1);
}

void dieWithStatus(const char *preamble, MxStatus status)
{
	MxStatusPrint(preamble, status);
	exit(-1);
}

void dieIfBad(const char *preamble, MxStatus status)
{
	if (MxStatusFailed(status))
	{
		MxStatusPrint(preamble, status);
		exit(-1);
	}
}