// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 1 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 1; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 1 * sizeof(PyObject *));
            memcpy(python_pars + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 1)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 1 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 1; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 1 * sizeof(PyObject *));
                memcpy(python_pars+1 + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 1)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 1);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 1, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 1);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 1);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 2 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 2; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 2 * sizeof(PyObject *));
            memcpy(python_pars + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 2)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 2 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 2; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 2 * sizeof(PyObject *));
                memcpy(python_pars+1 + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 2)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 2);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 2, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 2);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 2);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 3; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 3 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 3; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 3 * sizeof(PyObject *));
            memcpy(python_pars + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 3)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 3 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 3; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 3 * sizeof(PyObject *));
                memcpy(python_pars+1 + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 3)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 3);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 3, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 3);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 3);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 4; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 4 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 4; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 4 * sizeof(PyObject *));
            memcpy(python_pars + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 4)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 4 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 4; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 4 * sizeof(PyObject *));
                memcpy(python_pars+1 + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 4)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 4);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 4, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 4);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 4);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 5; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 5 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 5; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 5 * sizeof(PyObject *));
            memcpy(python_pars + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 5)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 5 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 5; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 5 * sizeof(PyObject *));
                memcpy(python_pars+1 + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 5)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 5);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 5, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 5);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 5);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS6(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 6; i++)
    {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 6 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 6; i++)
            {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 6 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 6 * sizeof(PyObject *));
            memcpy(python_pars + 6, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++)
            {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 6)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL)
        {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 6 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 6; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 6 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 6 * sizeof(PyObject *));
                memcpy(python_pars+1 + 6, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 6)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 6 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (6 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 6 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (6 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 6);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 6, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 6);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            6
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 6);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS1(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS1(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS2(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    2
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS2(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    2
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS2(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS3(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 3; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    3
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS3(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    3
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS3(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS3(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS3(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS3(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS4(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 4; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS4(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS4(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    4
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS4(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS4(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS4(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    4
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS4(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS4(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS4(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS4(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS4(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS5(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 5; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS5(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS5(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    5
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS5(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS5(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS5(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    5
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS5(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS5(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS5(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS5(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS5(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
extern PyObject *modulecode___main__(char const *);
extern PyObject *modulecode___parents_main__(char const *);
extern PyObject *modulecode_cairo(char const *);
extern PyObject *modulecode_cleanop(char const *);
extern PyObject *modulecode_dialogs(char const *);
extern PyObject *modulecode_gio(char const *);
extern PyObject *modulecode_glib(char const *);
extern PyObject *modulecode_glib$option(char const *);
extern PyObject *modulecode_gobject(char const *);
extern PyObject *modulecode_gobject$constants(char const *);
extern PyObject *modulecode_gobject$propertyhelper(char const *);
extern PyObject *modulecode_gtk(char const *);
extern PyObject *modulecode_gtk$_lazyutils(char const *);
extern PyObject *modulecode_gtk$deprecation(char const *);
extern PyObject *modulecode_multiprocessing$$45$postLoad(char const *);
extern PyObject *modulecode_multiprocessing$$45$preLoad(char const *);
extern PyObject *modulecode_powerset(char const *);
extern PyObject *modulecode_runop(char const *);
extern PyObject *modulecode_runtime(char const *);
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    {"PyExtHash", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"PyExtLic", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"PyExtOb", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"__main__", modulecode___main__, 0, 0, },
    {"__parents_main__", modulecode___parents_main__, 0, 0, },
    {"_bsddb", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_msi", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_socket", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"atk", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"background", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"cairo", modulecode_cairo, 0, 0, NUITKA_PACKAGE_FLAG},
    {"cairo._cairo", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"cleanop", modulecode_cleanop, 0, 0, },
    {"csv", NULL, 50431, 13367, NUITKA_BYTECODE_FLAG},
    {"ctypes", NULL, 63798, 20015, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes.util", NULL, 83813, 7756, NUITKA_BYTECODE_FLAG},
    {"dialogs", modulecode_dialogs, 0, 0, },
    {"getopt", NULL, 91569, 6596, NUITKA_BYTECODE_FLAG},
    {"gio", modulecode_gio, 0, 0, NUITKA_PACKAGE_FLAG},
    {"gio._gio", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"glib", modulecode_glib, 0, 0, NUITKA_PACKAGE_FLAG},
    {"glib._glib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"glib.option", modulecode_glib$option, 0, 0, },
    {"gobject", modulecode_gobject, 0, 0, NUITKA_PACKAGE_FLAG},
    {"gobject._gobject", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"gobject.constants", modulecode_gobject$constants, 0, 0, },
    {"gobject.propertyhelper", modulecode_gobject$propertyhelper, 0, 0, },
    {"gtk", modulecode_gtk, 0, 0, NUITKA_PACKAGE_FLAG},
    {"gtk._gtk", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"gtk._lazyutils", modulecode_gtk$_lazyutils, 0, 0, },
    {"gtk.deprecation", modulecode_gtk$deprecation, 0, 0, },
    {"lockhash", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"multiprocessing", NULL, 98165, 8327, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing-postLoad", modulecode_multiprocessing$$45$postLoad, 0, 0, },
    {"multiprocessing-preLoad", modulecode_multiprocessing$$45$preLoad, 0, 0, },
    {"multiprocessing.forking", NULL, 106492, 14189, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.reduction", NULL, 120681, 5921, NUITKA_BYTECODE_FLAG},
    {"optparse", NULL, 126602, 53423, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 180025, 25426, NUITKA_BYTECODE_FLAG},
    {"pango", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"pangocairo", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"platform", NULL, 205451, 36863, NUITKA_BYTECODE_FLAG},
    {"powerset", modulecode_powerset, 0, 0, },
    {"pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"re", NULL, 242314, 13300, NUITKA_BYTECODE_FLAG},
    {"runop", modulecode_runop, 0, 0, },
    {"runprocess", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"runscan", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"runsetup", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"runthread", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"runtime", modulecode_runtime, 0, 0, NUITKA_PACKAGE_FLAG},
    {"runupdate", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"select", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"string", NULL, 255614, 20196, NUITKA_BYTECODE_FLAG},
    {"subprocess", NULL, 275810, 41664, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 317474, 42204, NUITKA_BYTECODE_FLAG},
    {"types", NULL, 359678, 2682, NUITKA_BYTECODE_FLAG},
    {"unicodedata", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"verify", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"warnings", NULL, 362360, 13501, NUITKA_BYTECODE_FLAG},
    {"BaseHTTPServer", NULL, 375861, 21589, NUITKA_BYTECODE_FLAG},
    {"Bastion", NULL, 397450, 6577, NUITKA_BYTECODE_FLAG},
    {"CGIHTTPServer", NULL, 404027, 11037, NUITKA_BYTECODE_FLAG},
    {"Canvas", NULL, 415064, 15156, NUITKA_BYTECODE_FLAG},
    {"ConfigParser", NULL, 430220, 24910, NUITKA_BYTECODE_FLAG},
    {"Cookie", NULL, 455130, 22459, NUITKA_BYTECODE_FLAG},
    {"Dialog", NULL, 477589, 1887, NUITKA_BYTECODE_FLAG},
    {"DocXMLRPCServer", NULL, 479476, 9737, NUITKA_BYTECODE_FLAG},
    {"FileDialog", NULL, 489213, 9606, NUITKA_BYTECODE_FLAG},
    {"FixTk", NULL, 498819, 1975, NUITKA_BYTECODE_FLAG},
    {"HTMLParser", NULL, 500794, 13566, NUITKA_BYTECODE_FLAG},
    {"MimeWriter", NULL, 514360, 7311, NUITKA_BYTECODE_FLAG},
    {"Queue", NULL, 521671, 9276, NUITKA_BYTECODE_FLAG},
    {"ScrolledText", NULL, 530947, 2631, NUITKA_BYTECODE_FLAG},
    {"SimpleDialog", NULL, 533578, 4276, NUITKA_BYTECODE_FLAG},
    {"SimpleHTTPServer", NULL, 537854, 8003, NUITKA_BYTECODE_FLAG},
    {"SimpleXMLRPCServer", NULL, 545857, 22674, NUITKA_BYTECODE_FLAG},
    {"SocketServer", NULL, 568531, 23765, NUITKA_BYTECODE_FLAG},
    {"StringIO", NULL, 592296, 11377, NUITKA_BYTECODE_FLAG},
    {"Tix", NULL, 603673, 94389, NUITKA_BYTECODE_FLAG},
    {"Tkconstants", NULL, 698062, 2233, NUITKA_BYTECODE_FLAG},
    {"Tkdnd", NULL, 700295, 12687, NUITKA_BYTECODE_FLAG},
    {"Tkinter", NULL, 712982, 197044, NUITKA_BYTECODE_FLAG},
    {"UserDict", NULL, 910026, 8587, NUITKA_BYTECODE_FLAG},
    {"UserList", NULL, 918613, 6399, NUITKA_BYTECODE_FLAG},
    {"UserString", NULL, 925012, 14516, NUITKA_BYTECODE_FLAG},
    {"_LWPCookieJar", NULL, 939528, 5494, NUITKA_BYTECODE_FLAG},
    {"_MozillaCookieJar", NULL, 945022, 4433, NUITKA_BYTECODE_FLAG},
    {"__future__", NULL, 949455, 4254, NUITKA_BYTECODE_FLAG},
    {"_abcoll", NULL, 953709, 25024, NUITKA_BYTECODE_FLAG},
    {"_osx_support", NULL, 978733, 11699, NUITKA_BYTECODE_FLAG},
    {"_pyio", NULL, 990432, 63545, NUITKA_BYTECODE_FLAG},
    {"_strptime", NULL, 1053977, 14767, NUITKA_BYTECODE_FLAG},
    {"_threading_local", NULL, 1068744, 6551, NUITKA_BYTECODE_FLAG},
    {"_weakrefset", NULL, 1075295, 9430, NUITKA_BYTECODE_FLAG},
    {"abc", NULL, 1084725, 6080, NUITKA_BYTECODE_FLAG},
    {"aifc", NULL, 1090805, 30108, NUITKA_BYTECODE_FLAG},
    {"anydbm", NULL, 1120913, 2777, NUITKA_BYTECODE_FLAG},
    {"argparse", NULL, 1123690, 63568, NUITKA_BYTECODE_FLAG},
    {"ast", NULL, 1187258, 12826, NUITKA_BYTECODE_FLAG},
    {"asynchat", NULL, 1200084, 8661, NUITKA_BYTECODE_FLAG},
    {"asyncore", NULL, 1208745, 18537, NUITKA_BYTECODE_FLAG},
    {"atexit", NULL, 1227282, 2165, NUITKA_BYTECODE_FLAG},
    {"audiodev", NULL, 1229447, 8326, NUITKA_BYTECODE_FLAG},
    {"bdb", NULL, 1237773, 18788, NUITKA_BYTECODE_FLAG},
    {"binhex", NULL, 1256561, 15196, NUITKA_BYTECODE_FLAG},
    {"bisect", NULL, 1271757, 3038, NUITKA_BYTECODE_FLAG},
    {"bsddb", NULL, 1274795, 12257, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"bsddb.db", NULL, 1287052, 579, NUITKA_BYTECODE_FLAG},
    {"bsddb.dbobj", NULL, 1287631, 18280, NUITKA_BYTECODE_FLAG},
    {"bsddb.dbrecio", NULL, 1305911, 5233, NUITKA_BYTECODE_FLAG},
    {"bsddb.dbshelve", NULL, 1311144, 12754, NUITKA_BYTECODE_FLAG},
    {"bsddb.dbtables", NULL, 1323898, 24222, NUITKA_BYTECODE_FLAG},
    {"bsddb.dbutils", NULL, 1348120, 1609, NUITKA_BYTECODE_FLAG},
    {"cProfile", NULL, 1349729, 6230, NUITKA_BYTECODE_FLAG},
    {"calendar", NULL, 1355959, 27386, NUITKA_BYTECODE_FLAG},
    {"cgi", NULL, 1383345, 32210, NUITKA_BYTECODE_FLAG},
    {"cgitb", NULL, 1415555, 12086, NUITKA_BYTECODE_FLAG},
    {"chunk", NULL, 1427641, 5538, NUITKA_BYTECODE_FLAG},
    {"cmd", NULL, 1433179, 13926, NUITKA_BYTECODE_FLAG},
    {"code", NULL, 1447105, 10246, NUITKA_BYTECODE_FLAG},
    {"codeop", NULL, 1457351, 6539, NUITKA_BYTECODE_FLAG},
    {"collections", NULL, 1463890, 25353, NUITKA_BYTECODE_FLAG},
    {"colorsys", NULL, 1489243, 3943, NUITKA_BYTECODE_FLAG},
    {"commands", NULL, 1493186, 2431, NUITKA_BYTECODE_FLAG},
    {"compileall", NULL, 1495617, 6979, NUITKA_BYTECODE_FLAG},
    {"compiler", NULL, 1502596, 1284, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"compiler.ast", NULL, 1503880, 70172, NUITKA_BYTECODE_FLAG},
    {"compiler.consts", NULL, 1574052, 724, NUITKA_BYTECODE_FLAG},
    {"compiler.future", NULL, 1574776, 2988, NUITKA_BYTECODE_FLAG},
    {"compiler.misc", NULL, 1577764, 3630, NUITKA_BYTECODE_FLAG},
    {"compiler.pyassem", NULL, 1581394, 25591, NUITKA_BYTECODE_FLAG},
    {"compiler.pycodegen", NULL, 1606985, 55642, NUITKA_BYTECODE_FLAG},
    {"compiler.symbols", NULL, 1662627, 17389, NUITKA_BYTECODE_FLAG},
    {"compiler.syntax", NULL, 1680016, 1844, NUITKA_BYTECODE_FLAG},
    {"compiler.transformer", NULL, 1681860, 46997, NUITKA_BYTECODE_FLAG},
    {"compiler.visitor", NULL, 1728857, 4129, NUITKA_BYTECODE_FLAG},
    {"contextlib", NULL, 1732986, 4386, NUITKA_BYTECODE_FLAG},
    {"cookielib", NULL, 1737372, 54279, NUITKA_BYTECODE_FLAG},
    {"copy", NULL, 1791651, 12056, NUITKA_BYTECODE_FLAG},
    {"csv", NULL, 50431, 13367, NUITKA_BYTECODE_FLAG},
    {"ctypes", NULL, 63798, 20015, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes._endian", NULL, 1803707, 2269, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib", NULL, 1805976, 303, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes.macholib.dyld", NULL, 1806279, 5699, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.dylib", NULL, 1811978, 2287, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.framework", NULL, 1814265, 2597, NUITKA_BYTECODE_FLAG},
    {"ctypes.util", NULL, 83813, 7756, NUITKA_BYTECODE_FLAG},
    {"ctypes.wintypes", NULL, 1816862, 5882, NUITKA_BYTECODE_FLAG},
    {"curses", NULL, 1822744, 1530, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"dbhash", NULL, 1824274, 700, NUITKA_BYTECODE_FLAG},
    {"decimal", NULL, 1824974, 170085, NUITKA_BYTECODE_FLAG},
    {"difflib", NULL, 1995059, 61671, NUITKA_BYTECODE_FLAG},
    {"dircache", NULL, 2056730, 1548, NUITKA_BYTECODE_FLAG},
    {"dis", NULL, 2058278, 6180, NUITKA_BYTECODE_FLAG},
    {"distutils", NULL, 2064458, 373, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.archive_util", NULL, 2064831, 7408, NUITKA_BYTECODE_FLAG},
    {"distutils.bcppcompiler", NULL, 2072239, 7829, NUITKA_BYTECODE_FLAG},
    {"distutils.ccompiler", NULL, 2080068, 36547, NUITKA_BYTECODE_FLAG},
    {"distutils.cmd", NULL, 2116615, 16611, NUITKA_BYTECODE_FLAG},
    {"distutils.command", NULL, 2133226, 652, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.command.bdist", NULL, 2133878, 5181, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_dumb", NULL, 2139059, 5010, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_msi", NULL, 2144069, 23920, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_rpm", NULL, 2167989, 17692, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_wininst", NULL, 2185681, 10735, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build", NULL, 2196416, 5089, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_clib", NULL, 2201505, 6375, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_ext", NULL, 2207880, 19262, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_py", NULL, 2227142, 11420, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_scripts", NULL, 2238562, 4491, NUITKA_BYTECODE_FLAG},
    {"distutils.command.check", NULL, 2243053, 6200, NUITKA_BYTECODE_FLAG},
    {"distutils.command.clean", NULL, 2249253, 3143, NUITKA_BYTECODE_FLAG},
    {"distutils.command.config", NULL, 2252396, 12582, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install", NULL, 2264978, 16747, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_data", NULL, 2281725, 3121, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_egg_info", NULL, 2284846, 3713, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_headers", NULL, 2288559, 2253, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_lib", NULL, 2290812, 6716, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_scripts", NULL, 2297528, 2955, NUITKA_BYTECODE_FLAG},
    {"distutils.command.register", NULL, 2300483, 10149, NUITKA_BYTECODE_FLAG},
    {"distutils.command.sdist", NULL, 2310632, 16603, NUITKA_BYTECODE_FLAG},
    {"distutils.command.upload", NULL, 2327235, 6290, NUITKA_BYTECODE_FLAG},
    {"distutils.config", NULL, 2333525, 3527, NUITKA_BYTECODE_FLAG},
    {"distutils.core", NULL, 2337052, 7631, NUITKA_BYTECODE_FLAG},
    {"distutils.cygwinccompiler", NULL, 2344683, 9757, NUITKA_BYTECODE_FLAG},
    {"distutils.debug", NULL, 2354440, 241, NUITKA_BYTECODE_FLAG},
    {"distutils.dep_util", NULL, 2354681, 3152, NUITKA_BYTECODE_FLAG},
    {"distutils.dir_util", NULL, 2357833, 6743, NUITKA_BYTECODE_FLAG},
    {"distutils.dist", NULL, 2364576, 39228, NUITKA_BYTECODE_FLAG},
    {"distutils.emxccompiler", NULL, 2403804, 7414, NUITKA_BYTECODE_FLAG},
    {"distutils.errors", NULL, 2411218, 6177, NUITKA_BYTECODE_FLAG},
    {"distutils.extension", NULL, 2417395, 7384, NUITKA_BYTECODE_FLAG},
    {"distutils.fancy_getopt", NULL, 2424779, 11845, NUITKA_BYTECODE_FLAG},
    {"distutils.file_util", NULL, 2436624, 6717, NUITKA_BYTECODE_FLAG},
    {"distutils.filelist", NULL, 2443341, 10670, NUITKA_BYTECODE_FLAG},
    {"distutils.log", NULL, 2454011, 2718, NUITKA_BYTECODE_FLAG},
    {"distutils.msvc9compiler", NULL, 2456729, 21372, NUITKA_BYTECODE_FLAG},
    {"distutils.msvccompiler", NULL, 2478101, 17387, NUITKA_BYTECODE_FLAG},
    {"distutils.spawn", NULL, 2495488, 6356, NUITKA_BYTECODE_FLAG},
    {"distutils.sysconfig", NULL, 2501844, 13121, NUITKA_BYTECODE_FLAG},
    {"distutils.text_file", NULL, 2514965, 9196, NUITKA_BYTECODE_FLAG},
    {"distutils.unixccompiler", NULL, 2524161, 7535, NUITKA_BYTECODE_FLAG},
    {"distutils.util", NULL, 2531696, 14279, NUITKA_BYTECODE_FLAG},
    {"distutils.version", NULL, 2545975, 7125, NUITKA_BYTECODE_FLAG},
    {"distutils.versionpredicate", NULL, 2553100, 5499, NUITKA_BYTECODE_FLAG},
    {"doctest", NULL, 2558599, 83077, NUITKA_BYTECODE_FLAG},
    {"dumbdbm", NULL, 2641676, 6481, NUITKA_BYTECODE_FLAG},
    {"dummy_thread", NULL, 2648157, 5311, NUITKA_BYTECODE_FLAG},
    {"dummy_threading", NULL, 2653468, 1272, NUITKA_BYTECODE_FLAG},
    {"email", NULL, 2654740, 2834, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email._parseaddr", NULL, 2657574, 13682, NUITKA_BYTECODE_FLAG},
    {"email.base64mime", NULL, 2671256, 5290, NUITKA_BYTECODE_FLAG},
    {"email.charset", NULL, 2676546, 13448, NUITKA_BYTECODE_FLAG},
    {"email.encoders", NULL, 2689994, 2189, NUITKA_BYTECODE_FLAG},
    {"email.errors", NULL, 2692183, 3446, NUITKA_BYTECODE_FLAG},
    {"email.feedparser", NULL, 2695629, 11444, NUITKA_BYTECODE_FLAG},
    {"email.generator", NULL, 2707073, 10277, NUITKA_BYTECODE_FLAG},
    {"email.header", NULL, 2717350, 13574, NUITKA_BYTECODE_FLAG},
    {"email.iterators", NULL, 2730924, 2333, NUITKA_BYTECODE_FLAG},
    {"email.message", NULL, 2733257, 28435, NUITKA_BYTECODE_FLAG},
    {"email.mime", NULL, 2761692, 117, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email.mime.application", NULL, 2761809, 1561, NUITKA_BYTECODE_FLAG},
    {"email.mime.audio", NULL, 2763370, 2881, NUITKA_BYTECODE_FLAG},
    {"email.mime.base", NULL, 2766251, 1093, NUITKA_BYTECODE_FLAG},
    {"email.mime.image", NULL, 2767344, 2026, NUITKA_BYTECODE_FLAG},
    {"email.mime.message", NULL, 2769370, 1425, NUITKA_BYTECODE_FLAG},
    {"email.mime.multipart", NULL, 2770795, 1646, NUITKA_BYTECODE_FLAG},
    {"email.mime.nonmultipart", NULL, 2772441, 865, NUITKA_BYTECODE_FLAG},
    {"email.mime.text", NULL, 2773306, 1285, NUITKA_BYTECODE_FLAG},
    {"email.parser", NULL, 2774591, 3781, NUITKA_BYTECODE_FLAG},
    {"email.quoprimime", NULL, 2778372, 8777, NUITKA_BYTECODE_FLAG},
    {"email.utils", NULL, 2787149, 9034, NUITKA_BYTECODE_FLAG},
    {"filecmp", NULL, 2796183, 9514, NUITKA_BYTECODE_FLAG},
    {"fileinput", NULL, 2805697, 14727, NUITKA_BYTECODE_FLAG},
    {"fnmatch", NULL, 2820424, 3576, NUITKA_BYTECODE_FLAG},
    {"formatter", NULL, 2824000, 18785, NUITKA_BYTECODE_FLAG},
    {"fpformat", NULL, 2842785, 4624, NUITKA_BYTECODE_FLAG},
    {"fractions", NULL, 2847409, 19530, NUITKA_BYTECODE_FLAG},
    {"ftplib", NULL, 2866939, 34227, NUITKA_BYTECODE_FLAG},
    {"genericpath", NULL, 2901166, 3454, NUITKA_BYTECODE_FLAG},
    {"getopt", NULL, 91569, 6596, NUITKA_BYTECODE_FLAG},
    {"getpass", NULL, 2904620, 4701, NUITKA_BYTECODE_FLAG},
    {"gettext", NULL, 2909321, 15364, NUITKA_BYTECODE_FLAG},
    {"glob", NULL, 2924685, 2895, NUITKA_BYTECODE_FLAG},
    {"gzip", NULL, 2927580, 15037, NUITKA_BYTECODE_FLAG},
    {"hashlib", NULL, 2942617, 7014, NUITKA_BYTECODE_FLAG},
    {"heapq", NULL, 2949631, 14467, NUITKA_BYTECODE_FLAG},
    {"hmac", NULL, 2964098, 4484, NUITKA_BYTECODE_FLAG},
    {"hotshot", NULL, 2968582, 3421, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"hotshot.log", NULL, 2972003, 5465, NUITKA_BYTECODE_FLAG},
    {"hotshot.stats", NULL, 2977468, 3205, NUITKA_BYTECODE_FLAG},
    {"hotshot.stones", NULL, 2980673, 1138, NUITKA_BYTECODE_FLAG},
    {"htmlentitydefs", NULL, 2981811, 6354, NUITKA_BYTECODE_FLAG},
    {"htmllib", NULL, 2988165, 19846, NUITKA_BYTECODE_FLAG},
    {"httplib", NULL, 3008011, 36444, NUITKA_BYTECODE_FLAG},
    {"ihooks", NULL, 3044455, 21019, NUITKA_BYTECODE_FLAG},
    {"imaplib", NULL, 3065474, 44858, NUITKA_BYTECODE_FLAG},
    {"imghdr", NULL, 3110332, 4750, NUITKA_BYTECODE_FLAG},
    {"importlib", NULL, 3115082, 1479, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"imputil", NULL, 3116561, 15425, NUITKA_BYTECODE_FLAG},
    {"inspect", NULL, 3131986, 39679, NUITKA_BYTECODE_FLAG},
    {"io", NULL, 3171665, 3555, NUITKA_BYTECODE_FLAG},
    {"json", NULL, 3175220, 13922, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"json.decoder", NULL, 3189142, 12027, NUITKA_BYTECODE_FLAG},
    {"json.encoder", NULL, 3201169, 13617, NUITKA_BYTECODE_FLAG},
    {"json.scanner", NULL, 3214786, 2206, NUITKA_BYTECODE_FLAG},
    {"json.tool", NULL, 3216992, 1276, NUITKA_BYTECODE_FLAG},
    {"keyword", NULL, 3218268, 2087, NUITKA_BYTECODE_FLAG},
    {"lib2to3", NULL, 3220355, 114, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.btm_matcher", NULL, 3220469, 5767, NUITKA_BYTECODE_FLAG},
    {"lib2to3.btm_utils", NULL, 3226236, 7490, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_base", NULL, 3233726, 7169, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_util", NULL, 3240895, 14505, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes", NULL, 3255400, 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.fixes.fix_apply", NULL, 3255520, 1860, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_asserts", NULL, 3257380, 1538, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_basestring", NULL, 3258918, 784, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_buffer", NULL, 3259702, 941, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_callable", NULL, 3260643, 1484, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_dict", NULL, 3262127, 3741, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_except", NULL, 3265868, 2981, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exec", NULL, 3268849, 1409, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_execfile", NULL, 3270258, 2050, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exitfunc", NULL, 3272308, 2724, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_filter", NULL, 3275032, 2247, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_funcattrs", NULL, 3277279, 1105, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_future", NULL, 3278384, 910, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_getcwdu", NULL, 3279294, 917, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_has_key", NULL, 3280211, 3175, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_idioms", NULL, 3283386, 4494, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_import", NULL, 3287880, 3215, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports", NULL, 3291095, 5322, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports2", NULL, 3296417, 616, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_input", NULL, 3297033, 1125, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_intern", NULL, 3298158, 1596, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_isinstance", NULL, 3299754, 1829, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools", NULL, 3301583, 1782, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools_imports", NULL, 3303365, 2007, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_long", NULL, 3305372, 832, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_map", NULL, 3306204, 3031, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_metaclass", NULL, 3309235, 6551, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_methodattrs", NULL, 3315786, 1129, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ne", NULL, 3316915, 973, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_next", NULL, 3317888, 3507, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_nonzero", NULL, 3321395, 1077, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_numliterals", NULL, 3322472, 1237, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_operator", NULL, 3323709, 5067, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_paren", NULL, 3328776, 1534, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_print", NULL, 3330310, 2715, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raise", NULL, 3333025, 2489, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raw_input", NULL, 3335514, 927, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reduce", NULL, 3336441, 1253, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_renames", NULL, 3337694, 2428, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_repr", NULL, 3340122, 1007, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_set_literal", NULL, 3341129, 1976, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_standarderror", NULL, 3343105, 844, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_sys_exc", NULL, 3343949, 1693, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_throw", NULL, 3345642, 1987, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_tuple_params", NULL, 3347629, 5400, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_types", NULL, 3353029, 2176, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_unicode", NULL, 3355205, 1704, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_urllib", NULL, 3356909, 7107, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ws_comma", NULL, 3364016, 1373, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xrange", NULL, 3365389, 3039, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xreadlines", NULL, 3368428, 1143, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_zip", NULL, 3369571, 1337, NUITKA_BYTECODE_FLAG},
    {"lib2to3.main", NULL, 3370908, 9773, NUITKA_BYTECODE_FLAG},
    {"lib2to3.patcomp", NULL, 3380681, 6598, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2", NULL, 3387279, 161, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.pgen2.conv", NULL, 3387440, 8151, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.driver", NULL, 3395591, 5325, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.grammar", NULL, 3400916, 5983, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.literals", NULL, 3406899, 1990, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.parse", NULL, 3408889, 7168, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.pgen", NULL, 3416057, 12005, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.token", NULL, 3428062, 2275, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.tokenize", NULL, 3430337, 16862, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pygram", NULL, 3447199, 1391, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pytree", NULL, 3448590, 29915, NUITKA_BYTECODE_FLAG},
    {"lib2to3.refactor", NULL, 3478505, 23744, NUITKA_BYTECODE_FLAG},
    {"linecache", NULL, 3502249, 3234, NUITKA_BYTECODE_FLAG},
    {"logging", NULL, 3505483, 56967, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"logging.config", NULL, 3562450, 25678, NUITKA_BYTECODE_FLAG},
    {"logging.handlers", NULL, 3588128, 38961, NUITKA_BYTECODE_FLAG},
    {"macpath", NULL, 3627089, 7578, NUITKA_BYTECODE_FLAG},
    {"macurl2path", NULL, 3634667, 2216, NUITKA_BYTECODE_FLAG},
    {"mailbox", NULL, 3636883, 75623, NUITKA_BYTECODE_FLAG},
    {"mailcap", NULL, 3712506, 7006, NUITKA_BYTECODE_FLAG},
    {"markupbase", NULL, 3719512, 9224, NUITKA_BYTECODE_FLAG},
    {"md5", NULL, 3728736, 365, NUITKA_BYTECODE_FLAG},
    {"mhlib", NULL, 3729101, 33419, NUITKA_BYTECODE_FLAG},
    {"mimetools", NULL, 3762520, 8116, NUITKA_BYTECODE_FLAG},
    {"mimetypes", NULL, 3770636, 18196, NUITKA_BYTECODE_FLAG},
    {"mimify", NULL, 3788832, 11918, NUITKA_BYTECODE_FLAG},
    {"modulefinder", NULL, 3800750, 18583, NUITKA_BYTECODE_FLAG},
    {"msilib", NULL, 3819333, 19128, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"msilib.schema", NULL, 3838461, 59838, NUITKA_BYTECODE_FLAG},
    {"msilib.sequence", NULL, 3898299, 6264, NUITKA_BYTECODE_FLAG},
    {"msilib.text", NULL, 3904563, 18477, NUITKA_BYTECODE_FLAG},
    {"multifile", NULL, 3923040, 5337, NUITKA_BYTECODE_FLAG},
    {"multiprocessing", NULL, 98165, 8327, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.connection", NULL, 3928377, 14249, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.dummy", NULL, 3942626, 5352, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.dummy.connection", NULL, 3947978, 2696, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.forking", NULL, 106492, 14189, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.heap", NULL, 3950674, 6801, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.managers", NULL, 3957475, 37952, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.pool", NULL, 3995427, 22193, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.process", NULL, 4017620, 9357, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.queues", NULL, 4026977, 11361, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.reduction", NULL, 120681, 5921, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.sharedctypes", NULL, 4038338, 8494, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.synchronize", NULL, 4046832, 10871, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.util", NULL, 4057703, 9982, NUITKA_BYTECODE_FLAG},
    {"mutex", NULL, 4067685, 2472, NUITKA_BYTECODE_FLAG},
    {"netrc", NULL, 4070157, 4619, NUITKA_BYTECODE_FLAG},
    {"new", NULL, 4074776, 849, NUITKA_BYTECODE_FLAG},
    {"nntplib", NULL, 4075625, 20816, NUITKA_BYTECODE_FLAG},
    {"ntpath", NULL, 4096441, 13012, NUITKA_BYTECODE_FLAG},
    {"nturl2path", NULL, 4109453, 1792, NUITKA_BYTECODE_FLAG},
    {"numbers", NULL, 4111245, 13694, NUITKA_BYTECODE_FLAG},
    {"opcode", NULL, 4124939, 6119, NUITKA_BYTECODE_FLAG},
    {"optparse", NULL, 126602, 53423, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 180025, 25426, NUITKA_BYTECODE_FLAG},
    {"os2emxpath", NULL, 4131058, 4473, NUITKA_BYTECODE_FLAG},
    {"pdb", NULL, 4135531, 43030, NUITKA_BYTECODE_FLAG},
    {"pickle", NULL, 4178561, 37942, NUITKA_BYTECODE_FLAG},
    {"pickletools", NULL, 4216503, 56942, NUITKA_BYTECODE_FLAG},
    {"pipes", NULL, 4273445, 9220, NUITKA_BYTECODE_FLAG},
    {"pkgutil", NULL, 4282665, 18760, NUITKA_BYTECODE_FLAG},
    {"platform", NULL, 205451, 36863, NUITKA_BYTECODE_FLAG},
    {"plistlib", NULL, 4301425, 18966, NUITKA_BYTECODE_FLAG},
    {"popen2", NULL, 4320391, 8937, NUITKA_BYTECODE_FLAG},
    {"poplib", NULL, 4329328, 13171, NUITKA_BYTECODE_FLAG},
    {"posixfile", NULL, 4342499, 7584, NUITKA_BYTECODE_FLAG},
    {"posixpath", NULL, 4350083, 11297, NUITKA_BYTECODE_FLAG},
    {"pprint", NULL, 4361380, 10091, NUITKA_BYTECODE_FLAG},
    {"profile", NULL, 4371471, 16255, NUITKA_BYTECODE_FLAG},
    {"pstats", NULL, 4387726, 24705, NUITKA_BYTECODE_FLAG},
    {"pty", NULL, 4412431, 4908, NUITKA_BYTECODE_FLAG},
    {"py_compile", NULL, 4417339, 6385, NUITKA_BYTECODE_FLAG},
    {"pyclbr", NULL, 4423724, 9578, NUITKA_BYTECODE_FLAG},
    {"pydoc", NULL, 4433302, 91288, NUITKA_BYTECODE_FLAG},
    {"pydoc_data", NULL, 4524590, 117, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"pydoc_data.topics", NULL, 4524707, 395971, NUITKA_BYTECODE_FLAG},
    {"random", NULL, 4920678, 25349, NUITKA_BYTECODE_FLAG},
    {"repr", NULL, 4946027, 5292, NUITKA_BYTECODE_FLAG},
    {"rexec", NULL, 4951319, 23864, NUITKA_BYTECODE_FLAG},
    {"rfc822", NULL, 4975183, 31542, NUITKA_BYTECODE_FLAG},
    {"rlcompleter", NULL, 5006725, 5930, NUITKA_BYTECODE_FLAG},
    {"robotparser", NULL, 5012655, 7832, NUITKA_BYTECODE_FLAG},
    {"runpy", NULL, 5020487, 8300, NUITKA_BYTECODE_FLAG},
    {"sched", NULL, 5028787, 4941, NUITKA_BYTECODE_FLAG},
    {"sets", NULL, 5033728, 16607, NUITKA_BYTECODE_FLAG},
    {"sgmllib", NULL, 5050335, 15193, NUITKA_BYTECODE_FLAG},
    {"sha", NULL, 5065528, 408, NUITKA_BYTECODE_FLAG},
    {"shelve", NULL, 5065936, 10113, NUITKA_BYTECODE_FLAG},
    {"shlex", NULL, 5076049, 7470, NUITKA_BYTECODE_FLAG},
    {"shutil", NULL, 5083519, 18260, NUITKA_BYTECODE_FLAG},
    {"site", NULL, 5101779, 16407, NUITKA_BYTECODE_FLAG},
    {"smtpd", NULL, 5118186, 15733, NUITKA_BYTECODE_FLAG},
    {"smtplib", NULL, 5133919, 29982, NUITKA_BYTECODE_FLAG},
    {"sndhdr", NULL, 5163901, 7264, NUITKA_BYTECODE_FLAG},
    {"socket", NULL, 5171165, 15995, NUITKA_BYTECODE_FLAG},
    {"sqlite3", NULL, 5187160, 151, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"sqlite3.dbapi2", NULL, 5187311, 2655, NUITKA_BYTECODE_FLAG},
    {"sqlite3.dump", NULL, 5189966, 2048, NUITKA_BYTECODE_FLAG},
    {"sre", NULL, 5192014, 506, NUITKA_BYTECODE_FLAG},
    {"ssl", NULL, 5192520, 31291, NUITKA_BYTECODE_FLAG},
    {"stat", NULL, 5223811, 2693, NUITKA_BYTECODE_FLAG},
    {"statvfs", NULL, 5226504, 607, NUITKA_BYTECODE_FLAG},
    {"string", NULL, 255614, 20196, NUITKA_BYTECODE_FLAG},
    {"stringold", NULL, 5227111, 12406, NUITKA_BYTECODE_FLAG},
    {"subprocess", NULL, 275810, 41664, NUITKA_BYTECODE_FLAG},
    {"sunau", NULL, 5239517, 18157, NUITKA_BYTECODE_FLAG},
    {"sunaudio", NULL, 5257674, 1954, NUITKA_BYTECODE_FLAG},
    {"symbol", NULL, 5259628, 3008, NUITKA_BYTECODE_FLAG},
    {"symtable", NULL, 5262636, 11528, NUITKA_BYTECODE_FLAG},
    {"sysconfig", NULL, 5274164, 17413, NUITKA_BYTECODE_FLAG},
    {"tabnanny", NULL, 5291577, 8135, NUITKA_BYTECODE_FLAG},
    {"tarfile", NULL, 5299712, 74650, NUITKA_BYTECODE_FLAG},
    {"telnetlib", NULL, 5374362, 22974, NUITKA_BYTECODE_FLAG},
    {"tempfile", NULL, 5397336, 20020, NUITKA_BYTECODE_FLAG},
    {"test", NULL, 5417356, 111, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"test.pystone", NULL, 5417467, 7921, NUITKA_BYTECODE_FLAG},
    {"textwrap", NULL, 5425388, 11826, NUITKA_BYTECODE_FLAG},
    {"this", NULL, 5437214, 1207, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 317474, 42204, NUITKA_BYTECODE_FLAG},
    {"timeit", NULL, 5438421, 12052, NUITKA_BYTECODE_FLAG},
    {"tkColorChooser", NULL, 5450473, 1387, NUITKA_BYTECODE_FLAG},
    {"tkCommonDialog", NULL, 5451860, 1478, NUITKA_BYTECODE_FLAG},
    {"tkFileDialog", NULL, 5453338, 5073, NUITKA_BYTECODE_FLAG},
    {"tkFont", NULL, 5458411, 7020, NUITKA_BYTECODE_FLAG},
    {"tkMessageBox", NULL, 5465431, 3831, NUITKA_BYTECODE_FLAG},
    {"tkSimpleDialog", NULL, 5469262, 8975, NUITKA_BYTECODE_FLAG},
    {"toaiff", NULL, 5478237, 3078, NUITKA_BYTECODE_FLAG},
    {"token", NULL, 5481315, 3783, NUITKA_BYTECODE_FLAG},
    {"tokenize", NULL, 5485098, 14135, NUITKA_BYTECODE_FLAG},
    {"trace", NULL, 5499233, 22614, NUITKA_BYTECODE_FLAG},
    {"traceback", NULL, 5521847, 11571, NUITKA_BYTECODE_FLAG},
    {"ttk", NULL, 5533418, 61779, NUITKA_BYTECODE_FLAG},
    {"tty", NULL, 5595197, 1294, NUITKA_BYTECODE_FLAG},
    {"turtle", NULL, 5596491, 138320, NUITKA_BYTECODE_FLAG},
    {"unittest", NULL, 5734811, 2951, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"unittest.case", NULL, 5737762, 39878, NUITKA_BYTECODE_FLAG},
    {"unittest.loader", NULL, 5777640, 11262, NUITKA_BYTECODE_FLAG},
    {"unittest.main", NULL, 5788902, 7958, NUITKA_BYTECODE_FLAG},
    {"unittest.result", NULL, 5796860, 7796, NUITKA_BYTECODE_FLAG},
    {"unittest.runner", NULL, 5804656, 7536, NUITKA_BYTECODE_FLAG},
    {"unittest.signals", NULL, 5812192, 2708, NUITKA_BYTECODE_FLAG},
    {"unittest.suite", NULL, 5814900, 10355, NUITKA_BYTECODE_FLAG},
    {"unittest.util", NULL, 5825255, 4470, NUITKA_BYTECODE_FLAG},
    {"urllib", NULL, 5829725, 49965, NUITKA_BYTECODE_FLAG},
    {"urllib2", NULL, 5879690, 46607, NUITKA_BYTECODE_FLAG},
    {"urlparse", NULL, 5926297, 14379, NUITKA_BYTECODE_FLAG},
    {"user", NULL, 5940676, 1711, NUITKA_BYTECODE_FLAG},
    {"uu", NULL, 5942387, 4279, NUITKA_BYTECODE_FLAG},
    {"uuid", NULL, 5946666, 22593, NUITKA_BYTECODE_FLAG},
    {"warnings", NULL, 362360, 13501, NUITKA_BYTECODE_FLAG},
    {"wave", NULL, 5969259, 19758, NUITKA_BYTECODE_FLAG},
    {"weakref", NULL, 5989017, 15089, NUITKA_BYTECODE_FLAG},
    {"webbrowser", NULL, 6004106, 19546, NUITKA_BYTECODE_FLAG},
    {"whichdb", NULL, 6023652, 2223, NUITKA_BYTECODE_FLAG},
    {"wsgiref", NULL, 6025875, 716, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"wsgiref.handlers", NULL, 6026591, 16061, NUITKA_BYTECODE_FLAG},
    {"wsgiref.headers", NULL, 6042652, 7375, NUITKA_BYTECODE_FLAG},
    {"wsgiref.simple_server", NULL, 6050027, 6157, NUITKA_BYTECODE_FLAG},
    {"wsgiref.util", NULL, 6056184, 5925, NUITKA_BYTECODE_FLAG},
    {"wsgiref.validate", NULL, 6062109, 16651, NUITKA_BYTECODE_FLAG},
    {"xdrlib", NULL, 6078760, 9684, NUITKA_BYTECODE_FLAG},
    {"xml", NULL, 6088444, 1065, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom", NULL, 6089509, 6361, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom.NodeFilter", NULL, 6095870, 1103, NUITKA_BYTECODE_FLAG},
    {"xml.dom.domreg", NULL, 6096973, 3278, NUITKA_BYTECODE_FLAG},
    {"xml.dom.expatbuilder", NULL, 6100251, 32736, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minicompat", NULL, 6132987, 3476, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minidom", NULL, 6136463, 64555, NUITKA_BYTECODE_FLAG},
    {"xml.dom.pulldom", NULL, 6201018, 12860, NUITKA_BYTECODE_FLAG},
    {"xml.dom.xmlbuilder", NULL, 6213878, 16231, NUITKA_BYTECODE_FLAG},
    {"xml.etree", NULL, 6230109, 116, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.etree.ElementInclude", NULL, 6230225, 1939, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementPath", NULL, 6232164, 7485, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementTree", NULL, 6239649, 34351, NUITKA_BYTECODE_FLAG},
    {"xml.etree.cElementTree", NULL, 6274000, 163, NUITKA_BYTECODE_FLAG},
    {"xml.parsers", NULL, 6274163, 301, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.parsers.expat", NULL, 6274464, 274, NUITKA_BYTECODE_FLAG},
    {"xml.sax", NULL, 6274738, 3661, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.sax._exceptions", NULL, 6278399, 6076, NUITKA_BYTECODE_FLAG},
    {"xml.sax.expatreader", NULL, 6284475, 14578, NUITKA_BYTECODE_FLAG},
    {"xml.sax.handler", NULL, 6299053, 12895, NUITKA_BYTECODE_FLAG},
    {"xml.sax.saxutils", NULL, 6311948, 14523, NUITKA_BYTECODE_FLAG},
    {"xml.sax.xmlreader", NULL, 6326471, 18874, NUITKA_BYTECODE_FLAG},
    {"xmllib", NULL, 6345345, 26570, NUITKA_BYTECODE_FLAG},
    {"xmlrpclib", NULL, 6371915, 43272, NUITKA_BYTECODE_FLAG},
    {"zipfile", NULL, 6415187, 41396, NUITKA_BYTECODE_FLAG},
    {NULL, NULL, 0, 0, 0}
};

void setupMetaPathBasedLoader(void) {
    static bool init_done = false;

    if (init_done == false)
    {
        registerMetaPathBasedUnfreezer(meta_path_loader_entries);
        init_done = true;
    }
}
