//
//  MxFunctions.h
//  core_ds
//
//  Common function signatures
//

#ifndef core_ds_MxFunctions_h
#define core_ds_MxFunctions_h

#include "MxStatus.h"

// Iteration callback function. The iteration should terminate if
// this returns anything other than MxStatusOK
typedef MxStatus (*MxIteratorCallback)(const void *data, void *state);


// Order two values. Implementation should return:
// 		< 0  if first < second
//		0    if first == second
//		> 0  if first > second
typedef int (*MxCompareFunction)(const void *first, const void *second);


// Compare two values. Implementations should return non-0 if the
// values are equal, 0 otherwise...
typedef int (*MxEqualsFunction)(const void* first, const void *second);


// De-allocate the memory associated pointed at by the given pointer...
typedef void (*MxFreeFunction)(void *value);


// Signaure of a hash function
typedef unsigned long (*MxHashFunction)(const void *key);


// Provide some default implementation of common functions
void MxDefaultFreeFunction(void *data);
int MxDefaultCompareFunction(const void *first, const void *second);
int MxDefaultEqualsFunction(const void *first, const void *second);

int MxDefaultCompareIntFunction(const void *first, const void *second);

int MxDefaultCStrEqualsFunction(const void *first, const void *second);
int MxDefaultCStrCompareFunction(const void *fist, const void *second);

unsigned long MxDefaultHashFunction(const void *key);
unsigned long MxDefaultStringHashFunction(const void *key);


#endif
