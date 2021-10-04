/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */

#include "os.h"
#include "GetPassPhrase.h"
#include <string>
#include <termios.h>
#include <stdio.h>
using namespace std;

static struct termios tiosold, tiosnew;
/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &tiosold); /* grab old terminal i/o settings */
  tiosnew = tiosold; /* make new settings same as old settings */
  tiosnew.c_lflag &= ~ICANON; /* disable buffered i/o */
  tiosnew.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &tiosnew); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &tiosold);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo=0) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

string GetPassPhrase()
{
  FILE* uuid_file;
  char uuid[100];
  char* temp;
  uuid_file = fopen("/sys/devices/virtual/dmi/id/product_uuid", "r");
  if(uuid_file) {
    temp = fgets(uuid, 100, uuid_file);
    if(temp != nullptr) {
      for(size_t i = 0; i < 100; ++i) {
        if(uuid[i] == '\n') {
          uuid[i] = '\0';
          break;
        }
      }
   } 
  }
  string password(uuid);

  return password;
}




