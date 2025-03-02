/* C:B**************************************************************************
   This software is Copyright (c) 2025 Scott Marks -- Magnolia Heights R&D

   system is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   system is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with system.  If not, see <http://www.gnu.org/licenses/>.

   * C:E********************************************************************** */

// Credit and many thanks are due to Larry Yaeger for his very tidy third answer at
// https://stackoverflow.com/questions/1583234/c-system-function-how-to-collect-the-output-of-the-issued-command
// from which this was shamelessly adapted to the C++ "std" world.


#include "system.h"
#include <cstring>
#include <iostream>
#include <stdlib.h>

// Calling function must free the returned result.
std::string system(std::string command) {
  FILE* fp;
  char* line = NULL;
  // Following initialization is equivalent to char* result = ""; and just
  // initializes result to an empty string, only it works with
  // -Werror=write-strings and is so much less clear.
  char* result = (char*) calloc(1, 1);
  size_t len = 0;

  fflush(NULL);
  fp = popen(command.c_str(), "r");
  if (fp == NULL) {
    std::cerr << "Cannot execute command:" << std::endl << command << std::endl;
    return std::string(NULL);
  }
#define EXCESSIVE_LINE_LENGTH 4096
  while(getline(&line, &len, fp) != -1) {
    // +1 below to allow room for null terminator.
    const size_t line_len=strnlen(line,EXCESSIVE_LINE_LENGTH);
    result = (char*) realloc(result, strlen(result) + line_len + 1);
    strncpy(result + strlen(result), line, line_len);
    result[strlen(result)+line_len+1]='\0';
    free(line);
    line = NULL;
  }

  fflush(fp);
  if (pclose(fp) != 0) {
    perror("Cannot close stream.\n");
  }
  return std::string(result);
}
