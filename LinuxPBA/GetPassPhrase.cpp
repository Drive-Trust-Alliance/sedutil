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
#include <ncurses.h>
using namespace std;


string GetPassPhrase(const char *prompt, bool show_asterisk)
{
  const char BACKSPACE=127;
  const char RETURN=10;
  string password;
  unsigned char ch=0;
  LOG(D4) << "Enter GetPassPhrase" << endl;
  mvprintw(4,2,prompt);
  noecho(); // Disable echoing
  while((ch=getch())!=RETURN)
    {
      LOG(I) << "key value" << (uint16_t) ch << endl;
       if(ch==BACKSPACE)
         {
            if(password.length()!=0)
              {
                 if(show_asterisk)
                 printw("\b \b");
                 password.resize(password.length()-1);
              }
         }
       else if(ch!=27) // ignore 'escape' key
         {
             password+=ch;
             if(show_asterisk)
                 printw("*");
         }
    }

  echo();
  return password;
}
