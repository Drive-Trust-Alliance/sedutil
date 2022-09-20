//
//  TPerDriverMethodIndex.h
//  SedUserClient
//
//  Created by Scott Marks on 9/19/22.
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//

#ifndef TPerDriverMethodIndex_h
#define TPerDriverMethodIndex_h



#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)


#define __TPerDriverMethod__(index, method, nScalarInputs, nStructInputs, nScalarOutputs, nStructOutputs) index,

typedef enum TPerDriverMethodIndex {
#include "TPerDriverDispatchTable.inc"
    kSedUserClientNumberOfMethods // Must be last
} TPerDriverMethodIndex;

#undef __TPerDriverMethod__


#if defined(__cplusplus)
} // extern "C"
#endif // defined(__cplusplus)



#endif  // #ifndef TPerDriverMethodIndex_h
