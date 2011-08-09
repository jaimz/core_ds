//
//  MxStringBuffer.c
//  core_ds
//
//  Created by J O'Brien on 09/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "MxStatus.h"
#include "MxDebug.h"
#include "MxStringBuffer.h"


static inline size_t UserToGap(MxStringBufferRef buffer, size_t userCoord);
static inline size_t LastCharacterPoint(MxStringBufferRef buffer);
static inline size_t CurrentGapSize(MxStringBufferRef buffer);
static inline void InsertCharacter(MxStringBufferRef buffer, char c);
static MxStatus MxStringBufferEnsureSpace(MxStringBufferRef buffer, size_t count);
static MxStatus ExpandStorage(MxStringBufferRef buffer, size_t shortFall);
static MxStatus ExpandGap(MxStringBufferRef buffer, size_t amt);
static int MoveGapToPoint(MxStringBufferRef buffer);
static int MoveGapAndPoint(MxStringBufferRef buffer, size_t newPoint);
static int MoveGapToEnd(MxStringBufferRef buffer);

static MxStatus ValidateBuffer(MxStringBufferRef buffer);

// ::DEBUG::
static void PrintVisRep(MxStringBufferRef buffer);
// ::END DEBUG::



static inline size_t LastCharacterPoint(MxStringBufferRef buffer)
{
	return (buffer->bufLen - CurrentGapSize(buffer));
}

static inline size_t CurrentGapSize(MxStringBufferRef buffer)
{
	return (buffer->ge - buffer->gs);
}

static inline void InsertCharacter(MxStringBufferRef buffer, char c)
{
	buffer->storage[buffer->point] = c;
	buffer->point += 1;
	buffer->gs += 1;
	buffer->count += 1;
}

inline MxStatus MxStringBufferSetPoint(MxStringBufferRef buffer, size_t point)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	
	if (point > buffer->count)
		return MxStatusIndexOutOfRange;
	
	buffer->point = point;
	
	return MxStatusOK;
}


static int MoveGapToPoint(MxStringBufferRef buffer)
{
	// paranoia
	if (!buffer)
		return -1;
	
	size_t amt = 0;
	
	
	// Where the point is in the gap coord system...
	size_t gPoint = UserToGap(buffer, buffer->point);
	
	
	if (buffer->gs == gPoint)
	{
		// nothing
	}
	else if (buffer->ge < gPoint)
	{
		// The point is after the gap...
		// ++++++++++_________++++++++++
		//                        ^
		amt = (gPoint - buffer->ge);
		memcpy(buffer->storage + buffer->gs, buffer->storage + buffer->ge, (amt + sizeof(char)));
		buffer->gs += amt;
		buffer->ge += amt;
	}
	else if (buffer->gs > gPoint)
	{
		// The point is before the gap...
		// ++++++++++_________++++++++++
		//    ^
		amt = (buffer->gs - gPoint);
		
		size_t destIdx = buffer->ge - amt;
		memmove(buffer->storage + destIdx, buffer->storage + gPoint, (amt * sizeof(char)));
		
		buffer->gs = gPoint;
		buffer->ge -= amt;
	}
	
	return 0;
}

static int MoveGapAndPoint(MxStringBufferRef buffer, size_t newPoint)
{
	if (!buffer || newPoint > buffer->bufLen)
		return -1;
	
	buffer->point = newPoint;
	return MoveGapToPoint(buffer);
}

static inline int MoveGapToEnd(MxStringBufferRef buffer)
{
	MxStringBufferSetPoint(buffer, buffer->count);
	return MoveGapToPoint(buffer);
}


// ++++++++++_________++++++++++........................|
// ->
// ++++++++++_____________++++++++++....................|
static MxStatus ExpandGap(MxStringBufferRef buffer, size_t amt)
{
	MxStatus result = MxStatusOK;
	
	// Make sure we don't run out of underlying storage...
	if ((buffer->bufLen + amt) > buffer->capacity)
		result = ExpandStorage(buffer, buffer->bufLen + amt);
	
	
	
	if (result == MxStatusOK)
	{
		// Shuffle all the characters after the gap - they'll almost certainly
		// overlap so use memmove
		memmove(
				(buffer->storage + (buffer->ge + amt)),		
				(buffer->storage + buffer->ge), 
				((buffer->bufLen - buffer->ge) * sizeof(char))
				);
		
		buffer->ge += amt;
		
		buffer->bufLen += amt;
	}
	
	return result;
}


static MxStatus ExpandStorage(MxStringBufferRef buffer, size_t shortFall)
{
	
	// TODO: Possible overflow below...
	size_t newLen = buffer->capacity * 2;
	while (newLen <= shortFall)
		newLen *= 2;
	
	
	char *newStorage = (char *)calloc(newLen, sizeof(char));
	
	if (newStorage == NULL)
		return MxStatusNoMemory;
	
	memcpy(newStorage, buffer->storage, (buffer->capacity * sizeof(char)));
	
	char *tmp = buffer->storage;
	buffer->storage = newStorage;
	buffer->capacity = newLen;
	
	// ::DEBUG::
	MxDebug("Freeing old storage after expansion...\n");
	// ::END DEBUG::
	free(tmp);
	
	// point and gap pointers don't need to change
	return MxStatusOK;
}

// Map a coord in the user-space to its coord in the gap space...
static inline size_t UserToGap(MxStringBufferRef buffer, size_t userCoord)
{
	if (userCoord <= buffer->gs)
		return userCoord;
	else
		return (buffer->ge - buffer->gs) + userCoord;
}




MxStringBufferRef MxStringBufferCreate(void)
{
	MxStringBufferRef result = calloc(1, sizeof(MxStringBuffer));
	if (result != NULL)
		MxStringBufferInit(result);
	
	return result;
}

MxStringBufferRef MxStringBufferCreateWithValues(size_t capacity, size_t gapLen)
{
	MxStringBufferRef result = calloc(1, sizeof(MxStringBuffer));
	if (result)
		MxStringBufferInitWithValues(result, capacity, gapLen);
	
	return result;
}

MxStatus MxStringBufferInit(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	return MxStringBufferInitWithValues(buffer, MxStringBufferDefaultCapacity, MxStringBufferGapSize);	
}


MxStatus MxStringBufferInitWithValues(MxStringBufferRef buffer, size_t capacity, size_t gapLen)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxStringBufferWipe(buffer);
	if (status != MxStatusOK)
		return status;
	
	buffer->capacity = capacity;
	buffer->gs = 0;
	buffer->ge = gapLen;
	buffer->bufLen = gapLen;
	
	buffer->storage = calloc(capacity, sizeof(char));
	
	if (buffer->storage == NULL)
		status = MxStatusNoMemory;
	
	return status;
}

MxStatus MxStringBufferInitWithCharacters(MxStringBufferRef buffer, const char *contents, size_t length)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxStringBufferInitWithValues(buffer, length, length);
	if (status == MxStatusOK)
		status = MxStringBufferAppendCharacters(buffer, contents, length);
	
	return status;
}


MxStringBufferRef MxStringBufferCreateWithCStr(const char *cstr)
{
	MxStringBufferRef result = malloc(sizeof(MxStringBuffer));
	if (result != NULL)
		MxStringBufferInitWithCStr(result, cstr);
	
	return result;
}



MxStatus MxStringBufferInitWithCStr(MxStringBufferRef buffer, const char *cstr)
{
	if (buffer == NULL || cstr == NULL)
	{
		MxPrintDebug("MxStringBufferInitWithCStr given NULL arg", "");
		return MxStatusNullArgument;
	}
	
	
	MxStatus result = MxStringBufferInit(buffer);
	if (result != MxStatusOK)
	{
		MxPrintDebug("MxStringBufferInit returned bad result: %s", MxStatusMsg(result));
		return result;
	}
	
	result = MxStringBufferAppend(buffer, cstr);
	
	
	MxPrintDebug("MxStringBufferInit seems to have completed OK. Returning %d", result);
	return result;
}


MxStatus MxStringBufferWipe(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	if (buffer->storage != NULL)
	{
		// ::DEBUG:: 
        //		MxDebug("Freeing buffer storage\n");
		// ::END DEBUG::
		free(buffer->storage);
	}
	
	buffer->capacity = 0;
	buffer->storage = NULL;
	
	buffer->gs = 0;
	buffer->ge = MxStringBufferGapSize;
	buffer->point = 0;
	buffer->count = 0;
	buffer->bufLen = MxStringBufferGapSize;
	
	return MxStatusOK;
}


MxStatus MxStringBufferDestroy(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = MxStringBufferWipe(buffer);
	if (result == MxStatusOK)
	{
		// ::DEBUG::
        //		MxDebug("Freeing MxStringBufferRef\n");
		// ::END DEBUG::
		
		free(buffer);
	}
	else {
		// ::DEBUG::
		MxStatusError("Bad result wiping string buffer\n", result);
		// ::END DEBUG::
	}
    
    
	return MxStatusOK;
}


MxStatus MxStringBufferInsertAtPoint(MxStringBufferRef buffer, const char *toInsert, size_t index)
{
	if (toInsert == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = ValidateBuffer(buffer);
	
	if (result != MxStatusOK)
		return result;
	
	
	result = MxStringBufferSetPoint(buffer, index);
    //	result = MxStringBufferSetPoint(buffer, index);
	
	if (result != MxStatusOK)
		return result;
	
	if (index == buffer->capacity)
	{
		// insertion is at the very end of the capacity 
		result = ExpandStorage(buffer, strlen(toInsert));
		if (result != MxStatusOK)
			return result;
	}
	
	return MxStringBufferInsert(buffer, toInsert);
}


MxStatus MxStringBufferInsert(MxStringBufferRef buffer, const char *toInsert)
{
	return MxStringBufferInsertCharacters(buffer, toInsert, strlen(toInsert));
}

MxStatus MxStringBufferInsertCharacters(MxStringBufferRef buffer, const char *toInsert, size_t count)
{
	MxAssertPointer(buffer);
	
	if (toInsert == NULL)
		return MxStatusNullArgument;
	
	MxStatus result = ValidateBuffer(buffer);
	if (result != MxStatusOK)
	{
		return result;
	}
	
	
	if (MoveGapToPoint(buffer) == -1)
		return MxStatusUnknownError;
	
	size_t currGapSize = buffer->ge - buffer->gs;
	
    
	if (count > currGapSize)
		ExpandGap(buffer, (count + MxStringBufferGapSize));
    
	
	memcpy(buffer->storage + buffer->gs, toInsert, (count * sizeof(char)));
	buffer->gs += count;
	buffer->point = buffer->gs;
	buffer->count += count;
	
	
	return MxStatusOK;	
}

MxStatus MxStringBufferEnsureSpace(MxStringBufferRef buffer, size_t count)
{
	MxStatus result = ValidateBuffer(buffer);
	MxStatusCheck(result);
	
	
	size_t currGapSize = buffer->ge - buffer->gs;
	
	if (count > currGapSize)
		ExpandGap(buffer, (count + MxStringBufferGapSize));
	
	return MxStatusOK;
}

MxStatus MxStringBufferInsertInt(MxStringBufferRef buffer, int integer)
{
	MxStatus result = MxStatusOK;
	
	int i = integer;
	
	if (MoveGapToPoint(buffer) < 0)
		return MxStatusUnknownError;
	
    
	int char_count = 1;
	int biggest_power = 1;
	
	if (i < 0)
	{
		i = 0 - i;
		char_count += 1;
	}
	
	if (i > 0)
	{
		while (i / (biggest_power * 10))
		{
			biggest_power *= 10;
			char_count += 1;
		}
	}
	
    
	result = MxStringBufferEnsureSpace(buffer, char_count);
	MxStatusCheck(result);
	
	if (integer < 0)
		InsertCharacter(buffer, '-');
	
	while (biggest_power)
	{
		InsertCharacter(buffer, '0' + (i / biggest_power));
		i %= biggest_power;
		biggest_power /= 10;
	}
    
	return result;
}

MxStatus MxStringBufferRemove(MxStringBufferRef buffer, size_t startIndex, size_t endIndex)
{
	return MxStatusNotImplemented;
}


MxStatus MxStringBufferAppend(MxStringBufferRef buffer, const char *cstr)
{
	if (buffer == NULL || cstr == NULL)
		return MxStatusNullArgument;
	
	if (MoveGapToEnd(buffer) == -1)
		return MxStatusUnknownError;
	
	return MxStringBufferInsert(buffer, cstr);
}

MxStatus MxStringBufferAppendCharacters(MxStringBufferRef buffer, const char *characters, size_t count)
{
	if (buffer == NULL || characters == NULL)
		return MxStatusNullArgument;
	
	MxStatus status = MxStatusOK;
	if ((status = MxStringBufferSetPoint(buffer, 0)) != MxStatusOK)
		return status;
	
	return MxStringBufferInsertCharacters(buffer, characters, count);
}


MxStatus MxStringBufferPrepend(MxStringBufferRef buffer, const char *cstr)
{
	if (buffer == NULL || cstr == NULL)
		return MxStatusNullArgument;
	
	if (MoveGapAndPoint(buffer, 0) == -1)
		return MxStatusUnknownError;
	
	return MxStringBufferInsert(buffer, cstr);
}


MxStatus MxStringBufferDelete(MxStringBufferRef buffer, size_t amount)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	if (amount == 0)
		return MxStatusOK;
	
	MoveGapToPoint(buffer);
	if ((buffer->ge + amount) > buffer->bufLen)
		amount = buffer->bufLen - buffer->ge;
	
	buffer->ge += amount;
	buffer->count -= amount;
	
	return MxStatusOK;
}

MxStatus MxStringBufferDeleteFromPoint(MxStringBufferRef buffer, size_t point, size_t amount)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
    
	
	if (MoveGapAndPoint(buffer, point) == -1)
		return MxStatusUnknownError;
	
	
	return MxStringBufferDelete(buffer, amount);
}


MxStatus MxStringBufferBackspace(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	if (buffer->gs > 0)
	{
		buffer->gs -= 1;
		buffer->count -= 1;
	}
	
	return MxStatusOK;
}


MxStatus MxStringBufferFillFromFd(MxStringBufferRef string, int fd, size_t toRead)
{
	return MxStatusNotImplemented;
}


char *MxStringBufferAsCStr(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return NULL;
	
	if (MoveGapToEnd(buffer) == -1)
		return NULL;
	
	buffer->storage[buffer->gs] = '\0';
	
	return buffer->storage;
}


MxStatus MxStringBufferCopyToCStr(MxStringBufferRef buffer, char **result, size_t max)
{
	if (buffer == NULL || (result == NULL) || (*result == NULL && max == -1))
		return MxStatusNullArgument;
	
	size_t numToCopy = (buffer->count > max) ? buffer->count : max;
	
	if (max == -1)
	{
		*result = malloc(sizeof(char) * max);
		if (*result == NULL)
			return MxStatusNoMemory;
	}
	
	// easy case - if the gap is after the characters we're interested in
	// then it's just one memcpy...
	if (buffer->gs >= numToCopy)
	{
		memcpy(*result, buffer->storage, sizeof(char) * numToCopy);
	}
	else if (buffer->gs == 0)
	{
		// pretty easy case - gap is before text so still one memcpy and
		// we need to inc source pointer by the gap size...
		memcpy(*result, buffer->storage + (sizeof(char) * CurrentGapSize(buffer)), numToCopy);
	}
	else
	{
		// awkward (and most common) case - gap is in the text; we need to copy
		// the text in two chunks...
		
	}
	
	return MxStatusOK;
}

MxStatus MxStringBufferClear(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	// Delete everything...
	buffer->gs = 0;
	buffer->ge = buffer->bufLen;
	buffer->count = 0;
	buffer->point = 0;
	
	return MxStatusOK;
}


inline size_t MxStringBufferGetCount(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return -1;
	
	return buffer->count;
}

inline size_t MxStringBufferGetByteCount(MxStringBufferRef buffer)
{
	// NOTE: Don't do encdings yet - this is the same as GetCount
	
	if (buffer == NULL)
		return -1;
	
	return buffer->count;
}


static MxStatus ValidateBuffer(MxStringBufferRef buffer)
{
	if (buffer == NULL)
		return MxStatusNullArgument;
	
	if (buffer->capacity <= 0)
		return MxStatusInvalidStructure;
	
	return MxStatusOK;
}


// ::DEBUG::
void MxStringBufferPrintDiagnostic(MxStringBufferRef buffer)
{
	PrintVisRep(buffer);
	
	
	printf(
		   "{\n\tgs: %lu\n\tge: %lu\n\tpoint: %lu\n\tcount: %lu\n\tbuflen: %lu\n\tcapacity: %d\n}\n\n",
		   buffer->gs, buffer->ge, buffer->point, buffer->count, buffer->bufLen, (int)buffer->capacity
		   );
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
// ::END DEBUG::