#include "pyextob.h"

#ifdef PYEXTOB
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "..\License\include.h"
#else
#include "include.h"
#endif
#endif

#include "ob.h"
#include <stdio.h>

obfs::obfs()
{

}
obfs::~obfs()
{
}


void obfs::rstor(char * st1, char * sf)
{

	//char st2[17] = "FidelityPRO5    "; // array size is 17-byte but actual content is 16-byte
	//printf("supposed text in hex : ");
	//for (int i = 0; i < 16; i++) {
	//	printf("%02X", st2[i] & 0xff);
	//}	
	//printf("\n");

	// restore to original text
	for (int i = 0; i < 16; i++) {
		st1[i] = ((sf[i] & 0xff) ^ (SeedFL + i));
	}
	//if (0) {
	//	printf("restore text in hex : ");
	//	for (int i = 0; i < 16; i++) {
	//		printf("%02X", st1[i] & 0xff);
	//	}
	//	printf("\n");
	//	printf("restore text in char : \"");
	//
	//	for (int i = 0; i < 16; i++) {
	//		printf("%C", st1[i] & 0xff);
	//	}
	//	printf("\"\n");
	//}

}


void obfs::lic(int lev, char * lic_level)
{
	switch (lev)
	{
	case 1:
		rstor(lic_level, sfree);
		break;
	case 2:
		rstor(lic_level, sstd);
		break;
	case 4:
		rstor(lic_level, s5);
		break;
	case 16:
		rstor(lic_level, s25);
		break;
	case 32:
		rstor(lic_level, s100);
		break;
	case 64:
		rstor(lic_level, sunlmt);
		break;
	default:
		for (int i = 0; i<16; i++)
			lic_level[i] = ' ';
		break;
	}
}

void obfs::setaudpass(char * apass)
{
	rstor(apass, audit);
}


#ifdef PYEXTOB
PyObject* ob_function1(PyObject* self, PyObject* args)
{
	int i;
	char rstr[16]; 

	if (!PyArg_ParseTuple(args, "i", &i))
	{
		//goto error; // why use go to 
		return 0;
	};
	obfs ob;
	ob.lic(i, rstr); 
	return PyString_FromStringAndSize(rstr,16);
}


PyMethodDef obMethods[] =
{
	{ "get_str",(PyCFunction)ob_function1,METH_VARARGS,0 },
	{ 0,0,0,0 }  // leave it empty for now
};


PyMODINIT_FUNC
initPyExtOb(void)
{
	PyObject *m;

	m = Py_InitModule("PyExtOb", obMethods); // array of exported function 
	if (m == NULL)
		return;
}
#endif
