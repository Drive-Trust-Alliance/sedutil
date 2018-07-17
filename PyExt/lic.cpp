
#include <Python.h>


PyObject* lic_function1(PyObject* self, PyObject* args)
{
	return 0;
}


PyMethodDef licMethods[] =
{
	{ "get_lic",(PyCFunction)lic_function1,METH_VARARGS,0 },
	{ 0,0,0,0 }  
};


PyMODINIT_FUNC
initPyExtLic(void)
{
	PyObject *m;
	m = Py_InitModule("PyExtLic", licMethods); // array of exported function 
	if (m == NULL)
		return;
}

