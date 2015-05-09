/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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

using namespace std;
#include <ncurses.h>
#include <unistd.h>
#include <sys/reboot.h>
#include "log.h"
#include "GetPassPhrase.h"
#include "UnlockSEDs.h"

int main(int argc, char** argv) {
    CLog::Level() = CLog::FromInt(0);
    LOG(D4) << "Legacy PBA start" << endl;
    initscr();
    raw();
    attron(A_BOLD);
    mvprintw(1,20,"MSED LINUX Pre Boot Authorization \n");
    attroff(A_BOLD);
    string p = GetPassPhrase("Please enter pass-phrase to unlock OPAL drives: ");
    UnlockSEDs((char *)p.c_str());
#ifdef MSEDDEBUG
    getch();
#endif
    mvprintw(8,20,"Starting OS \n");
#ifndef MSEDDEBUG
    sync();
    reboot(RB_AUTOBOOT);
#endif
    endwin();
    return 0;
}

