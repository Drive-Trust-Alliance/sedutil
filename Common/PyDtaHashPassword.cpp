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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "..\License\include.h"
#else  // not Windows
// python.h is included in include.h and must be include python.h first, it is python bug
#include "includePython.h"
#endif // Windows

#include "os.h"
#include <iostream>
#include <iomanip>
#include "DtaHashPassword.h"
#include "DtaLexicon.h"
#include "DtaOptions.h"
#include "DtaDev.h"
#include "log.h"


extern sedutiloutput outputFormat;
sedutiloutput outputFormat = sedutilNormal;

#if 1 // defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
extern "C" {
#include "pbkdf2.h"
#include "sha1.h"
}
#endif


#include <stdio.h>


// hashing for logging ON OFF command



// credit
// https://www.codeproject.com/articles/99547/hex-strings-to-raw-data-and-back
//

inline unsigned char hex_digit_to_nybble(char ch)
{
	switch (ch)
	{
	case '0': return 0x0;
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0x4;
	case '5': return 0x5;
	case '6': return 0x6;
	case '7': return 0x7;
	case '8': return 0x8;
	case '9': return 0x9;
	case 'a':
	case 'A': return 0xa;
	case 'b':
	case 'B': return 0xb;
	case 'c':
	case 'C': return 0xc;
	case 'd':
	case 'D': return 0xd;
	case 'e':
	case 'E': return 0xe;
	case 'f':
	case 'F': return 0xf;
	default: return 0xff;  // throw std::invalid_argument();
	}
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable: 4996)
#endif // Windows


static
PyObject* hash_function1(__unused PyObject* self, PyObject* args)
{
	vector<uint8_t>  hashstr; // returned hash
	char * password;
	char * saltstr;
	vector<uint8_t> salt;
	uint32_t iter;
	uint8_t sz;
	uint8_t hp[128]; // converted ascii from hashstr

	memset(hp, 0, 128);
	//void hashpwd::DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	//unsigned int iter, uint8_t hashsize)
	if (!PyArg_ParseTuple(args, "ssib", &password, &saltstr,&iter, &sz)) // str:password,str:salt,int(iteration,short i:hash size
	{
		//goto error; // why use go to 
		return 0;
	};
	salt.clear();
	for (size_t jj = 0; jj < strnlen(saltstr, 255); jj++) salt.push_back((uint8_t)saltstr[jj]);
    printf("%s %s %d %d %zu %lu\n",password, saltstr,iter, sz, strnlen(saltstr, 255), salt.size());
 	DtaHashPassword(hashstr, password, salt, iter, sz);
	
	for (size_t ii = 0; ii < hashstr.size() - 2; ii += 1) { // first 2 byte of hash vector is header
		snprintf((char *)hp + (ii * 2), 4, "%02x", hashstr.at(ii + 2));
	}
#if PYTHON2
    return PyString_FromStringAndSize((char *)hp,sz*2);
#else // PYTHON2
    return PyBytes_FromStringAndSize((char *)hp,sz*2);
#endif // PYTHON2
}


static PyMethodDef hashMethods[] =
{
	{ "hashpwd", (PyCFunction)hash_function1, METH_VARARGS, "Compute Dta password hash" },
    {NULL, NULL, 0, NULL}
};

#if PYTHON2

PyMODINIT_FUNC initPyExtHash(void); // prototype
PyMODINIT_FUNC initPyExtHash(void)
{
    PyObject *m;

    m = Py_InitModule("PyExtHash", hashMethods); // array of exported function
    if (m == NULL)
        return;
}
    

#else // !PYTHON2, i.e. PYTHON3


static struct PyModuleDef hashDefinition = {
    PyModuleDef_HEAD_INIT,  // PyModuleDef_Base m_base;
    "PyExtHash",            // const char* m_name;
    "A Python module that computes Dta password hashes from C code.",  // const char* m_doc;
    -1,                     // Py_ssize_t m_size;
    hashMethods,            // PyMethodDef *m_methods;
    NULL,                   // struct PyModuleDef_Slot* m_slots;
    NULL,                   // traverseproc m_traverse;
    NULL,                   // inquiry m_clear;
    NULL,                   // freefunc m_free;
};


PyMODINIT_FUNC PyInit_PyExtHash(void); // prototype
PyMODINIT_FUNC PyInit_PyExtHash(void)
{
    Py_Initialize();
    return PyModule_Create(&hashDefinition);
}
    
#endif // PYTHON2
