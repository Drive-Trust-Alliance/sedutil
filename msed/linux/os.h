/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "MsedConstants.h"
#include "log.h"
// Why can't I find these??
#define TRUE 1
#define FALSE 0
// a few OS specific methods that need to be worked out
#define SNPRINTF snprintf
#define ALIGNED_ALLOC(alignment, length) aligned_alloc(alignment, length)
#define ALIGNED_FREE free
#define DEVICEMASK "/dev/sg%i"

