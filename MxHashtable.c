//
//  MxHashtable.c
//  core_ds
//
//  Created by J O'Brien on 08/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "MxStatus.h"
#include "MxFunctions.h"
#include "MxHashtable.h"

#include "MxDebug.h"


static MxStatus DestroyEntry(const void *vpair, void *vtable);
static MxPairRef CreatePair(const void *key, const void *value);
static void RemoveListNode(MxListNodeRef node);
static inline void ReplaceValueInPair(MxHashtableRef table, MxPairRef pair, const void *newValue);
static MxStatus PutInBucket(MxHashtableRef table, MxListRef bucket, const void *key, const void *value);

MxHashtableRef MxHashtableCreate(void) {
	MxHashtableRef table = (MxHashtableRef)malloc(sizeof(MxHashtable));
	
	if (table != NULL) {
		if (MxHashtableInit(table) != MxStatusOK)
			free(table);
	}
	
	return table;
}


MxHashtableRef MxHashtableCreateWithAllFunctions(MxHashFunction hashFunction, MxEqualsFunction equals, MxFreeFunction keyFree, MxFreeFunction valueFree)
{
	MxHashtableRef table = (MxHashtableRef)malloc(sizeof(MxHashtable));
	if (table != NULL)
	{
		if (MxHashtableInitWithAllFunctions(table, hashFunction, equals, keyFree, valueFree) != MxStatusOK)
			free(table);
	}
	
	return table;
}


MxHashtableRef MxHashtableCreateWithFunction(MxHashFunction hashFunction)
{
	return MxHashtableCreateWithAllFunctions(hashFunction, NULL, NULL, NULL);
}


MxStatus MxHashtableInit(MxHashtableRef table)
{
	if (table == NULL)
		return MxStatusNullArgument;
    
	return MxHashtableInitWithAllFunctions(table, MxDefaultHashFunction, MxDefaultCompareFunction, MxDefaultFreeFunction, MxDefaultFreeFunction);
}

MxStatus MxHashtableInitWithFunction(MxHashtableRef table, MxHashFunction hashFunction)
{
	return MxHashtableInitWithAllFunctions(table, hashFunction, MxDefaultEqualsFunction, MxDefaultFreeFunction, MxDefaultFreeFunction);
}

MxStatus MxHashtableInitWithAllFunctions(MxHashtableRef table, MxHashFunction hashFunction, MxEqualsFunction equals, MxFreeFunction keyFree, MxFreeFunction valueFree)
{
	if (table == NULL)
		return MxStatusNullArgument;
	
	int ctr, rollbackCtr;
	
	table->bucketCount = MxHashtableDefaultBucketCount;
	table->buckets = calloc(table->bucketCount, sizeof(MxList));
    
	if (table->buckets == NULL)
		return MxStatusNoMemory;
	
	MxStatus status = MxStatusOK;
	for (ctr = 0; ctr < table->bucketCount; ++ctr) {
		if ((status = MxListInitWithFunctions(table->buckets + ctr, free, NULL)) != MxStatusOK)
		{
			for (rollbackCtr = 0; rollbackCtr < ctr; ++rollbackCtr)
				MxListDelete(table->buckets + rollbackCtr);
			
			return status;
		}
	}
	
	table->hashFunction = hashFunction ? hashFunction : MxDefaultHashFunction;
	table->equalsFunction = equals ? equals : MxDefaultEqualsFunction;
	table->keyFreeFunction = keyFree ? keyFree : MxDefaultFreeFunction;
	table->valueFreeFunction = valueFree ? valueFree : MxDefaultFreeFunction;
	
	table->count = 0;
	
	return MxStatusOK;
}

MxStatus MxHashtableSetKeyFreeFunction(MxHashtableRef table, MxFreeFunction freeFunction)
{
	if (table == NULL) return MxStatusNullArgument;
	table->keyFreeFunction = freeFunction;
	
	return MxStatusOK;
}

MxStatus MxHashtableSetValueFreeFunction(MxHashtableRef table, MxFreeFunction freeFunction)
{
	if (table == NULL) return MxStatusNullArgument;
	table->valueFreeFunction = freeFunction;
	
	return MxStatusOK;
}


static MxStatus DestroyPairContents(const void *key, const void *value, void *t)
{
	MxHashtableRef table = (MxHashtableRef)t;
	
	if (table->keyFreeFunction)
		table->keyFreeFunction((void *)key);
	
	if (table->valueFreeFunction)
		table->valueFreeFunction((void *)value);
	
	
	return MxStatusOK;
}

MxStatus MxHashtableClear(MxHashtableRef table)
{
	if (table == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxStatusOK;
	
	if (table->count == 0)
		return status;
	
	if (table->keyFreeFunction || table->valueFreeFunction)
		status = MxHashtableIteratePairs(table, DestroyPairContents, table);
    
	MxListRef bucket;
	for (int ctr = 0; ctr < table->bucketCount; ++ctr) {
		bucket = table->buckets + ctr;
		if ((status = MxListClear(bucket)) != MxStatusOK)
			break;
	}
	
	table->count = 0;
	
	return status;
}


MxStatus MxHashtableWipe(MxHashtableRef table)
{
	if (table == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxHashtableClear(table);
	MxStatusCheck(status);
	
	free(table->buckets);
	
	return MxStatusOK;
}



MxStatus MxHashtableDelete(MxHashtableRef table)
{
	if (table == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxHashtableWipe(table);
	if (result != MxStatusOK)
		return result;
	
	free(table);
	
	return MxStatusOK;
}

static MxStatus DestroyEntry(const void *vpair, void *vtable)
{
	if (vpair == NULL || vtable == NULL)
		return MxStatusNullArgument;
	
	MxPairRef pair = (MxPairRef)vpair;
	MxHashtableRef table = (MxHashtableRef)vtable;
	
	
	if (table->keyFreeFunction)
		table->keyFreeFunction(pair->key);
    
	if (table->valueFreeFunction)
		table->valueFreeFunction(pair->value);
	
	return MxStatusOK;
}



static MxStatus PutInBucket(MxHashtableRef table, MxListRef bucket, const void *key, const void *value)
{
	MxListNodeRef node = bucket->sentinel->next;
	MxPairRef pair = NULL;
	
	while (node != bucket->sentinel)
	{
		pair = (MxPairRef)node->data;
		if (pair != NULL)
		{
			if (table->equalsFunction && table->equalsFunction(key, pair->key))
			{
				ReplaceValueInPair(table, pair, value);
				break;
			}
			else if (key == pair->key)
			{
				ReplaceValueInPair(table, pair, value);
				break;
			}
		}
		
		node = node->next;
	}
	
	MxStatus status = MxStatusOK;
	if (node == bucket->sentinel)
	{
		// Key was not found...
		MxPairRef pair = CreatePair(key, value);
		if (pair != NULL)
			status = MxListAppend(bucket, pair);
		
		if (status == MxStatusOK)
			table->count += 1;		
	}
	
	
	return status;
}

static inline void ReplaceValueInPair(MxHashtableRef table, MxPairRef pair, const void *newValue)
{
	if (table->valueFreeFunction)
		table->valueFreeFunction(pair->value);
	
	pair->value = (void *)newValue;
}

MxStatus MxHashtablePut(MxHashtableRef table, const void *key, const void *value)
{
	if (table == NULL || key == NULL || value == NULL)
		return MxStatusNullArgument;
	
	if (table->hashFunction == NULL)
		return MxStatusInvalidStructure;
    
	unsigned long hash = table->hashFunction(key);
	unsigned int bucketIdx = (unsigned int)(hash % ((unsigned long)table->bucketCount));
    
	MxListRef bucket = (MxListRef)table->buckets + bucketIdx;
    
	MxStatus result = PutInBucket(table, bucket, key, value);
	
	return result;
}

MxStatus MxHashtableContainsKey(MxHashtableRef table, const void *key)
{
	if (table == NULL || key == NULL)
		return MxStatusNullArgument;
	
	if (table->hashFunction == NULL)
		return MxStatusInvalidStructure;
	
	unsigned long hash = table->hashFunction(key);
	unsigned int bucketIdx = (unsigned int)(hash % ((unsigned long)table->bucketCount));
	
	MxListRef bucket = (MxListRef)table->buckets + bucketIdx;
	MxPairRef pair = NULL;
	MxListNodeRef node = bucket->sentinel->next;
	MxStatus result = MxStatusFalse;
	
	while (node != bucket->sentinel)
	{
		pair = (MxPairRef)node->data;
		if (table->equalsFunction)
		{
			// ::DEBUG::
            //			printf("Table got equals function...\n");
			// ::END DEBUG::
			if (table->equalsFunction(key, pair->key))
			{
				result = MxStatusTrue;
				break;
			}
			else
			{
                //				MxPrintDebug("No equals function%s", "");
                //				MxPrintDebug("...(%s, %s) not equal\n", key, pair->key);
			}
		}
		else if (key == pair->key)
		{
			result = MxStatusTrue;
			break;
		}
		
		node = node->next;
	}
	
	return result;
}


MxPairRef CreatePair(const void *key, const void *value)
{
	MxPairRef result = (MxPairRef)malloc(sizeof(MxPair));
	if (result != NULL)
	{
		result->key = (void *)key;
		result->value = (void *)value;
	}
	
	return result;
}




MxStatus MxHashtableGet(MxHashtableRef table, const void *key, void **result)
{
	if (table == NULL || key == NULL)
		return MxStatusNullArgument;
	
	if (table->hashFunction == NULL || table->equalsFunction == NULL)
		return MxStatusInvalidStructure;
	
	*result = NULL;
	int found = 0;
	
	if (table->count > 0)
	{
		unsigned long hash = table->hashFunction(key);
		unsigned int bucketIdx = (unsigned int)hash % table->bucketCount;
		
		MxListRef bucket = table->buckets + bucketIdx;
		MxListNodeRef node = bucket->sentinel->next;
		
		MxPairRef pair = NULL;
		while (node != bucket->sentinel) {
			pair = (MxPairRef)node->data;
			if (pair != NULL)
			{
				if (table->equalsFunction(key, pair->key)) {
					*result = pair->value;
					found = 1;
					break;
				}
			}
			
			node = node->next;
		}
	}
	
	return (found ? MxStatusOK : MxStatusNotFound);
}


MxStatus MxHashtableRemove(MxHashtableRef table, const void *key)
{
	if (table == NULL || key == NULL)
		return MxStatusNullArgument;
	
	if (table->hashFunction == NULL || table->equalsFunction == NULL)
		return MxStatusInvalidStructure;
    
	int found = 0;
	if (table->count > 0)
	{
		unsigned long hash = table->hashFunction(key);
		unsigned int bucketIdx = (unsigned int)hash % table->bucketCount;
		
		MxListRef bucket = table->buckets + bucketIdx;
		MxListNodeRef node = bucket->sentinel->next;
		
		MxPairRef pair = NULL;
		while (node != bucket->sentinel) {
			pair = (MxPairRef)node->data;
			if (pair != NULL)
			{
				
				if (table->equalsFunction(key, pair->key)) {
					found = 1;
					if (table->keyFreeFunction)
						table->keyFreeFunction(pair->key);
					
					if (table->valueFreeFunction)
						table->valueFreeFunction(pair->value);
					
					RemoveListNode(node);
					bucket->count -= 1; // naughty - depends on internal structure of MxList
					table->count -= 1;
					
					break;
				}
			}
			
			node = node->next;
		}
	}
	
	return (found ? MxStatusOK : MxStatusNotFound);
}

MxStatus MxHashtableTake(MxHashtableRef table, const void *key, void **result)
{
	if (table == NULL || key == NULL)
		return MxStatusNullArgument;
	
	if (table->hashFunction == NULL || table->equalsFunction == NULL)
		return MxStatusInvalidStructure;
	
	*result = NULL;
	int found = 0;
	
	if (table->count > 0)
	{
		unsigned long hash = table->hashFunction(key);
		unsigned int bucketIdx = (unsigned int)hash % table->bucketCount;
		
		MxListRef bucket = table->buckets + bucketIdx;
		MxListNodeRef node = bucket->sentinel->next;
		
		MxPairRef pair = NULL;
		while (node != bucket->sentinel) {
			pair = (MxPairRef)node->data;
			if (pair != NULL)
			{
				if (table->equalsFunction(key, pair->key)) {
					*result = pair->value;
					found = 1;
					
					if (table->keyFreeFunction)
						table->keyFreeFunction(pair->key);
					
					if (table->valueFreeFunction)
						table->valueFreeFunction(pair->value);
					
					RemoveListNode(node);
					bucket->count--; // naughty - depends on internal struycture of list
					
					break;
				}
			}
			
			node = node->next;
		}
	}
    
	MxStatus status = MxStatusNotFound;
	if (found)
	{
		table->count--;
		status = MxStatusOK;
	}
    
	return status;
}

static void RemoveListNode(MxListNodeRef node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	
	free(node);
}


MxStatus MxHashtableIterateKeys(MxHashtableRef table, MxIteratorCallback callback, void *state)
{
	if (table == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxStatusOK;
	
	if (table->count > 0)
	{
		MxListRef bucket;
		MxListNodeRef node;
		for (int ctr = 0; ctr < table->bucketCount; ++ctr)
		{
			bucket = table->buckets + ctr;
			node = bucket->sentinel->next;
			while (node != bucket->sentinel)
			{
				result = callback(((MxPairRef)node->data)->key, state);
				if (result != MxStatusOK)
					goto done;
				node = node->next;
			}
		}
	}
    
done:
	return result;
}

MxStatus MxHashtableIterateValues(MxHashtableRef table, MxIteratorCallback callback, void *state)
{
	if (table == NULL || callback == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxStatusOK;
	
	if (table->count > 0)
	{
		MxListRef bucket;
		MxListNodeRef node;
		for (int ctr = 0; ctr < table->bucketCount; ++ctr)
		{
			bucket = table->buckets + ctr;
			node = bucket->sentinel->next;
			while (node != bucket->sentinel)
			{
				result = callback(((MxPairRef)node->data)->value, state);
				if (result != MxStatusOK)
					goto done;
				node = node->next;
			}
		}
	}
	
done:
	return result;
}

int MxHashtableIteratePairs(MxHashtableRef table, MxPairIteratorCallback callback, void *state) {
	if (table == NULL || callback == NULL)
		return MxStatusNullArgument;
    
    
	MxListRef bucket;
	MxListNodeRef node;
    
	MxStatus result = 0;
	if (table->count > 0) {
		for (int ctr = 0; ctr < table->bucketCount; ++ctr) {
			bucket = table->buckets + ctr;
			node = bucket->sentinel->next;
			while (node != bucket->sentinel) {
				result = callback(((MxPairRef)node->data)->key, ((MxPairRef)node->data)->value, state);
				if (result != MxStatusOK)
					goto done;
				node = node->next;
			}
		}
	}
	
done:
	return result;
}

inline int MxHashtableGetCount(MxHashtableRef table)
{
	if (table == NULL) return MxStatusNullArgument;
	
	return table->count;
}

inline MxHashtableRef MxHashtableCreatePropertyMap()
{
	return MxHashtableCreateWithAllFunctions(MxDefaultStringHashFunction, MxDefaultCStrEqualsFunction, NULL, NULL);
}

inline MxStatus MxHashtableInitAsPropertyMap(MxHashtableRef table)
{
	return MxHashtableInitWithAllFunctions(table, MxDefaultStringHashFunction, MxDefaultCStrEqualsFunction, NULL, NULL);
}