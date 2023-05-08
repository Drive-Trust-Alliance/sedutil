#pragma once

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#message("DEBUG was defined, turning it off")
#undef _DEBUG
#endif

#define register
constexpr static auto iWillBreak =
#include <Python3/Python.h>

#ifdef _DEBUG_WAS_DEFINED
#message("DEBUG was defined, turning it back on")
#define _DEBUG 1
#endif

#if PYTHON_ABI_VERSION==2
#define PYTHON2
#message("Python2")
#elif PYTHON_ABI_VERSION==3
#define PYTHON3
#message("Python3")
#else
#error Python version?
#endif
