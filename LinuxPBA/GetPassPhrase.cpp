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

string GetPassPhrase(const char *prompt, bool show_asterisk)
{
  const char BACKSPACE=127;
  const char RETURN=10;
  string password;
  unsigned char ch=0;
  LOG(D4) << "Enter GetPassPhrase" << endl;
  printf("\n\n%s",prompt);
  while((ch=getch_())!=RETURN)
    {
//      LOG(I) << "key value" << (uint16_t) ch << endl;
       if(ch==BACKSPACE)
         {
            if(password.length()!=0)
              {
                 if(show_asterisk)
                 printf("\b \b");
                 password.resize(password.length()-1);
              }
         }
       else if(ch!=27) // ignore 'escape' key
         {
             password+=ch;
             if(show_asterisk)
                 printf("*");
         }
    }

  printf("\n");
  return password;
}




