//
//  utils.h
//  tests
//
//  Created by J O'Brien on 11/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef tests_utils_h
#define tests_utils_h

#include "MxStatus.h"

void die(const char *msg);
void dieWithStatus(const char *preamble, MxStatus status);
void dieIfBad(const char *preamble, MxStatus status);

#endif
