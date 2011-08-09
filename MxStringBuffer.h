//
//  MxStringBuffer.h
//  core_ds
//
//  An efficiently mutable text buffer - uses the gap algorithm for
//  efficient insertion andn removal.
//
//  Only works with ASCI encoding - should really be called MxByteBuffer
//
//  Created by J O'Brien on 08/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxStringBuffer_h
#define core_ds_MxStringBuffer_h

#include <stdio.h> // for BUFSIZ
#include <ctype.h> // for size_t

#include "MxStatus.h"

#define MxStringBufferGapSize (255)
#define MxStringBufferDefaultCapacity (BUFSIZ)

typedef struct _MxStringBuffer
{
	// Gap start
	size_t gs;
    
	// Gap end
	size_t ge;
    
	// Insertion point
	size_t point;
    
	// The number of characters in the buffer - not including the gap
	size_t count;
    
    
	// The the number of characters in the buffer plus the length of the gap...
	size_t bufLen;
    
    
	// The amount of storage in this buffer...
	size_t capacity;
    
	// The actual character data...
	char *storage;
    
} MxStringBuffer, *MxStringBufferRef;


/** Dynamically allocate an empty buffer */
MxStringBufferRef MxStringBufferCreate(void);

/** Initialise a previously allocated buffer */
MxStatus MxStringBufferInit(MxStringBufferRef buffer);

/** Dynamically allocate a buffer containing the \0-terminated (ASCII) cstring */
MxStringBufferRef MxStringBufferCreateWithCStr(const char *cstring);

/** Dynamically allocate a buffer containing a copy of the ASCII byte array 'contents'.
 'length' is the number of characters to copy */
MxStringBufferRef MxStringBufferCreateWithCharacters(const char *contents, size_t length);

/** Dynamically allocate a buffer with capacity 'capacity' and gap length 'gapLen' 
 Usually used for debug, the default values should do you fine.
 */
MxStringBufferRef MxStringBufferCreateWithValues(size_t capacity, size_t gapLen);


/** Initialise a previously allocated buffer with a copy of the \0-terminated 'cstr' */
MxStatus MxStringBufferInitWithCStr(MxStringBufferRef string, const char *cstr);

/**  Initialise a previously allocated buffer with a copy of the character array 'contents' */
MxStatus MxStringBufferInitWithCharacters(MxStringBufferRef buffer, const char *contents, size_t length);

/** Init a previously alloc'd buffer with the given capacity and gapLen */
MxStatus MxStringBufferInitWithValues(MxStringBufferRef buffer, size_t capacity, size_t gapLen);


/** Free the internal memory in the given buffer.
    Does *not* delete the buffer itself - use with a stack alloc'd
    buffer.
 */
MxStatus MxStringBufferWipe(MxStringBufferRef buffer);

/** Destroy a Dynamically alloc'd buffer */
MxStatus MxStringBufferDestroy(MxStringBufferRef buffer);


/** Set the buffer's insertion point */
MxStatus MxStringBufferSetPoint(MxStringBufferRef buffer, size_t point);

/** Insert a \0-terminated ASCII string at point 'index' */
MxStatus MxStringBufferInsertAtPoint(MxStringBufferRef buffer, const char *toInsert, size_t index);

/** Insert a \0-terminated ASCII string at the current point */
MxStatus MxStringBufferInsert(MxStringBufferRef buffer, const char *toInsert);

/** Insert 'count' ASCII characters from the array 'toInsert' at the buffer's point */
MxStatus MxStringBufferInsertCharacters(MxStringBufferRef buffer, const char *toInsert, size_t count);

/** Insert a string representation of 'integer' at the current point */
MxStatus MxStringBufferInsertInt(MxStringBufferRef buffer, int integer);

/** Remove the characters between 'startIndex' and 'endIndex' */
MxStatus MxStringBufferRemove(MxStringBufferRef buffer, size_t startIndex, size_t endIndex);

/** Append the \0-terminated ASCII string 'cstr' to the buffer */
MxStatus MxStringBufferAppend(MxStringBufferRef buffer, const char *cstr);

/** Append 'length' ASCII charactersfrom the char array 'characters' to the buffer */
MxStatus MxStringBufferAppendCharacters(MxStringBufferRef buffer, const char *characters, size_t length);

/** Prepend the \0-terminated c string to the buffer */
MxStatus MxStringBufferPrepend(MxStringBufferRef buffer, const char *cstr);

/** Copy 'length' number of ASCII characters from 'characters' to start of the buffer */
MxStatus MxStringBufferPrependCharacters(MxStringBufferRef buffer, const char *characters, size_t length);

/** Delete 'amount' characters starting at the buffer's current point. */
MxStatus MxStringBufferDelete(MxStringBufferRef buffer, size_t amount);
/** Delete 'amount' characters starting at 'point' */
MxStatus MxStringBufferDeleteFromPoint(MxStringBufferRef buffer, size_t point, size_t amount);
/** Backspace 'amount' characters from the buffer's current point */
MxStatus MxStringBufferBackspace(MxStringBufferRef buffer);

/** Read 'toRead' characters from 'fd' and append it to buffer at the current point */
MxStatus MxStringBufferFillFromFd(MxStringBufferRef string, int fd, size_t toRead);

/** Return buffer as a valid \0-terminated C ctring - this does NOT copy the
    buffer. The C string returned points to the actual buffer so should be treated as
    immutable. */
char * MxStringBufferAsCStr(MxStringBufferRef buffer);

/** Copy the contents of the buffer to the C string *resul. If the buffer is
    longer than 'max' only 'max' characters will be copied. */
MxStatus MxStringBufferCopyToCStr(MxStringBufferRef buffer, char** result, size_t max);

/** Get the number of characters in the buffer */
size_t MxStringBufferGetCount(MxStringBufferRef string);

/** Get the number of bytes in the buffer */
size_t MxStringBufferGetByteCount(MxStringBufferRef string);

MxStatus MxStringBufferClear(MxStringBufferRef buffer);

// ::DEBUG::
void MxStringBufferPrintDiagnostic(MxStringBufferRef buffer);
// ::END DEBUG::


#endif
