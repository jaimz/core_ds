//
//  MxDebug.h
//  core_ds
//
//  Created by J O'Brien on 06/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxDebug_h
#define core_ds_MxDebug_h

void MxDebugFunc(const char *file, int line, const char *fmt, ...);


#define MxDebugPosn __FILE__,  __LINE__

#define MxPrintDebug(fmt, ...) MxDebugFunc(__FILE__, __LINE__, fmt, __VA_ARGS__)

#ifdef DEBUG
#define MxDebug(...) MxDebugFunc(__FILE__, __LINE__, __VA_ARGS__);
#else
#define MxDebug(...)
#endif

#endif
