//
//  MxFunctions.c
//  core_ds
//

#include <stdlib.h>
#include <string.h>

#include "MxFunctions.h"



/* Default implementation of MxFreeFunction - calls the stdlib free */
void MxDefaultFreeFunction(void *data) { free(data); }


/*	Default MxEqualsFunction - tests for address equality */
int MxDefaultEqualsFunction(const void *first, const void *second) {
	return (first == second);
}


int MxDefaultCompareFunction(const void *first, const void *second) {
	if (first == second)
		return 0;
	
	if (first < second)
		return -1;
	
	return 1;
}



int MxDefaultCStrCompareFunction(const void *first, const void *second)
{
	return strcmp((const char *)first, (const char *)second);
}



int MxDefaultCStrEqualsFunction(const void *first, const void *second)
{
	return (strcmp((const char *)first, (const char *)second) == 0);
}



unsigned long MxDefaultHashFunction(const void *key) 
{
    return (unsigned long)key;
}


/* Use djb2 algorithm as default string hash */
unsigned long MxDefaultStringHashFunction(const void *key)
{
    unsigned long hash = 5381;
    int c;
    const char *str = (const char *)key;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}