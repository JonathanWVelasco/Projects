// Preprocessor directive
#ifndef SIMULATOR_H 
#define SIMULATOR_H

// header files
#include "datatypes.h"
#include "configops.h"
#include "metadataops.h"
#include "StringUtils.h"

// functions
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

#endif        // SIMULATOR_H