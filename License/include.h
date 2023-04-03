#pragma once

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif

#define register
#include "Python.h"

#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif




