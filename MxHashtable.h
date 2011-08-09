//
//  MxHashtable.h
//  core_ds
//
//  A chained hashtable
//
//  Created by J O'Brien on 08/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxHashtable_h
#define core_ds_MxHashtable_h

#include "MxStatus.h"
#include "MxFunctions.h"
#include "MxList.h"


#define MxHashtableDefaultBucketCount (89)

typedef struct _MxPair
{
    void *key;
    void *value;
} MxPair, *MxPairRef;

typedef struct _MxHashtable 
{
    unsigned int bucketCount;
    MxListRef buckets;
    
    int count;
    
    MxHashFunction hashFunction;
    MxEqualsFunction equalsFunction;
    MxFreeFunction keyFreeFunction;
    MxFreeFunction valueFreeFunction;
} MxHashtable, *MxHashtableRef;



// Signature of a key/value iterator callback.
// TODO: Should be in MxFunctions.h
typedef MxStatus (*MxPairIteratorCallback)(const void *key, const void *value, void *state);


// Dynamically create a hashtable.
MxHashtableRef MxHashtableCreate(void);
MxHashtableRef MxHashtableCreateWithFunction(MxHashFunction hashFunction);
MxHashtableRef MxHashtableCreateWithAllFunctions(MxHashFunction hashFunction, MxCompareFunction compare, MxFreeFunction keyFree, MxFreeFunction valueFree);


// Initialise a pre-allocated hashtable...
MxStatus MxHashtableInit(MxHashtableRef table);
MxStatus MxHashtableInitWithFunction(MxHashtableRef table, MxHashFunction hashFunction);
MxStatus MxHashtableInitWithAllFunctions(MxHashtableRef table, MxHashFunction hashFunction, MxCompareFunction compare, MxFreeFunction keyFree, MxFreeFunction valueFree);


// Set the function used to free memory consumed by a key
MxStatus MxHashtableSetKeyFreeFunction(MxHashtableRef table, MxFreeFunction freeFunction);
// Set the function used to free memory consumed by a value
MxStatus MxHashtableSetValueFreeFunction(MxHashtableRef table, MxFreeFunction freeFunction);

// Wipe the internal memory used by a table (i.e. dynamically alloc'd buckets
// Does NOT free the table reference itself (use with stack alloc'd tables)
MxStatus MxHashtableWipe(MxHashtableRef table);

// Free all the memory used by a table
// Use with dynamically alloc'd tables
MxStatus MxHashtableDelete(MxHashtableRef table);


// Store a value in the table against key 'key'
// returns MxStatusOK  if the value was stored
//         MxStatusNullArgument if table, key or value is NULL
//         MxStatusInvalidStructure  if 'table' does not have a hash function
MxStatus MxHashtablePut(MxHashtableRef table, const void *key, const void *value);

// Get the value stored against key 'key'. A refernce to the value is placed in *result.
// The value is NOT copied (i.e. changes via *result will change the value in 'table')
// returns MxStatusOK  if the value was retrieved
//         MxStatusNullArgument if table or key is NULL
//         MxStatusInvalidStructure  if 'table' does not have a hash or equals function
//         MxStatusNotFound if there is nothing against 'key' in the table
MxStatus MxHashtableGet(MxHashtableRef table, const void *key, void **result);

// Remove the value stored against 'key'. If the table has functions to free key and value
// memory they will be run.
// returns MxStatusOK  if the value was removed
//         MxStatusNullArgument if table or key is NULL
//         MxStatusInvalidStructure  if 'table' does not have a hash or equals function
//         MxStatusNotFound if there is nothing against 'key' in the table
MxStatus MxHashtableRemove(MxHashtableRef table, const void *key);

// Remove and return (in *result) the value stored against 'key'
// If the table has functions to free memory consumed by keys then 'key' will be freed
// returns MxStatusOK  if the value was removed
//         MxStatusNullArgument if table or key is NULL
//         MxStatusInvalidStructure  if 'table' does not have a hash or equals function
//         MxStatusNotFound if there is nothing against 'key' in the table
MxStatus MxHashtableTake(MxHashtableRef table, const void *key, void **result);


// Remove all values in the table. If the table had key/value free functions they will
// be run for each pair.
// returns
//         MxStatusNullArgument if table or key is NULL
MxStatus MxHashtableClear(MxHashtableRef table);

// Iterate through all the keys in the table, calling 'callback' for each key.
// If 'callback' returns anything other than MxStatusOK the iteration will terminate immediately.
// The opaque object 'state' will be passed back to the callback on each visit
// returns MxStatusOK  if the iteration completed, the status returned by the terminating callback otherwise
//         MxStatusNullArgument if table or callback is NULL
MxStatus MxHashtableIterateKeys(MxHashtableRef table, MxIteratorCallback callback, void *state);


// Iterate through all the values in the table, calling 'callback' for each value.
// If 'callback' returns anything other than MxStatusOK the iteration will terminate immediately.
// The opaque object 'state' will be passed back to the callback on each visit
// returns MxStatusOK  if the iteration completed, the status returned by the terminating callback otherwise
//         MxStatusNullArgument if table or callback is NULL
MxStatus MxHashtableIterateValues(MxHashtableRef table, MxIteratorCallback callback, void *state);

// Iterate through all the keys and values in the table, calling 'callback' for each pair.
// If 'callback' returns anything other than MxStatusOK the iteration will terminate immediately.
// The opaque object 'state' will be passed back to the callback on each visit
// returns MxStatusOK  if the iteration completed, the status returned by the terminating callback otherwise
//         MxStatusNullArgument if table or callback is NULL
MxStatus MxHashtableIteratePairs(MxHashtableRef table, MxPairIteratorCallback callback, void *state);


// Get the number of entries in this table
int MxHashtableGetCount(MxHashtableRef table);


// Return:
//          MxStatusTrue if the table contains the key
//          MxStatusFalse if the table does not contain the key
//          MxStatusNullArgument if table or key is NULL
//          MxStatusINvalidStructure if the table does not have a hash function
int MxHashtableContainsKey(MxHashtableRef table, const void *key);


// Dynamically create a hashtable tailored for storing string keys
MxHashtableRef MxHashtableCreatePropertyMap();

// Initialise a pre-alloc'd table to efficiently store string keys
MxStatus MxHashtableInitAsPropertyMap(MxHashtableRef table);

#endif
