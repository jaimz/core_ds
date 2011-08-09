//
//  MxStatus.h
//  core_ds
//
//  Created by J O'Brien on 06/08/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef core_ds_MxStatus_h
#define core_ds_MxStatus_h


typedef int MxStatus;

// 10000 status values ought to be enough for anybody
#define MxStatusValueLimit (10000)


#define MxStatusTrue (1)
#define MxStatusFalse (0)

#define MxStatusOK (-1)
#define MxStatusNoMemory (-2)
#define MxStatusNullArgument (-3)
#define MxStatusBadArgument (-4)
#define MxStatusIndexOutOfRange (-5)
#define MxStatusNotImplemented (-6)
#define MxStatusUnknownError (-7)
#define MxStatusIllegalArgument (-8)
#define MxStatusReadError (-9)
#define MxStatusEOF (-10)
#define MxStatusCouldNotOpen (-11)
#define MxStatusInvalidStructure (-12)
#define MxStatusWriteError (-13)
#define MxStatusNotFound (-14)
#define MxStatusEqual (-15)
#define MxStatusLess (-16)
#define MxStatusMore (-17)
#define MxStatusConfigValueMissing (-18)
#define MxStatusUnixError (-19)
#define MxStatusFilenameTooLong (-20)
#define MxStatusIncomplete (-21)
#define MxStatusExists (-22)
#define MxStatusIterationBroken (-23);

#define MxAssertPointer(structPointer) if (structPointer == NULL) { return MxStatusNullArgument; }

#define MxStatusFailed(status) ((status != MxStatusOK) && (status != MxStatusTrue) && (status != MxStatusFalse))

#define MxStatusCheck(result) {if (MxStatusFailed(result)) { return result; }}


char *MxStatusMsg(MxStatus status);
void MxStatusPrint(const char *preamble, MxStatus status);
void MxStatusError(const char *preamble, MxStatus status);
void MxStatusErrorWithStdErr(const char *preamble, MxStatus status);


#endif
