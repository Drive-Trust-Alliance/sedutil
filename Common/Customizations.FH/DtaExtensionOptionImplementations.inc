/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

// Operating specific or customizing option implementations can be included in the
// body of the switch statement processing options by entering them here like this:
//
//  case sedutiloption::catchFireAndBurn:
//      LOG(D) << "Run in circles! Scream and shout!";
//      system("shutdown -h NOW");
//      return 127;
//      break;


case sedutiloption::MBRRead:
    LOG(D) << "Read shadow MBR  to file " << argv[opts.device];;
    return d->MBRRead(argv[opts.password], argv[opts.pbafile], atol(argv[opts.startpos]), atol(argv[opts.len]));
    break;

case sedutiloption::createUSB:
    LOG(D) << "create bootable USB drive " << argv[opts.pbafile] << " to " << argv[opts.device];
    return diskUSBwrite(argv[opts.device], argv[opts.devusb], _com_util::ConvertBSTRToString(m_lv->getf2s()));
    break;

case sedutiloption::pbaValid:
    LOG(D) << "PBA image validation " << argv[opts.device];;
    return d->pbaValid(argv[opts.password]);
    break;
    
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#define OSNAME "linux"
#elif defined(APPLE) || defined(_APPLE) || defined(__APPLE__)
#define OSNAME "macOS"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define OSNAME "windows"
#else
#define OSNAME "unknownOS";
#endif
case sedutiloption::version:
    LOG(D) << "print version number ";
    printf("Opal Lock Version : 0.9.8.%s.%s 20220621-A001 PBA.0.9.5.linux.%s 20220218-B001\n", OSNAME, GIT_VERSION, GIT_VERSION_PBA);
    return 0;
    break;
