//
//  MxDebug.c
//  core_ds
//

#include <stdio.h>
#include <stdarg.h>

#include "MxDebug.h"


void MxDebugFunc(const char *file, int line, const char *fmt, ...)
{
	va_list ap;
	
	fprintf(stderr, "MX-DEBUG: %s (%d): ", file, line);
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}