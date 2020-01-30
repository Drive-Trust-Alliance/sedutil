/* Generated code for Python module 'multiprocessing-postLoad'
 * created by Nuitka version 0.6.6
 *
 * This code is in part copyright 2019 Kay Hayen.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nuitka/prelude.h"

#include "__helpers.h"

/* The "_module_multiprocessing$$45$postLoad" is a Python object pointer of module type.
 *
 * Note: For full compatibility with CPython, every module variable access
 * needs to go through it except for cases where the module cannot possibly
 * have changed in the mean time.
 */

PyObject *module_multiprocessing$$45$postLoad;
PyDictObject *moduledict_multiprocessing$$45$postLoad;

/* The declarations of module constants used, if any. */
static PyObject *const_str_plain_register;
static PyObject *const_str_plain_im_func;
extern PyObject *const_str_plain_getattr;
static PyObject *const_str_digest_a5e1b9d507c3b6e0344a6503f9256148;
extern PyObject *const_str_plain___name__;
static PyObject *const_str_plain_im_class;
extern PyObject *const_str_plain___doc__;
extern PyObject *const_tuple_empty;
static PyObject *const_str_digest_83826bb58b51044e5d41a9cc646c18d6;
static PyObject *const_str_digest_a2691ee6974e7c106bcdaf1f585bded9;
extern PyObject *const_str_plain_m;
extern PyObject *const_str_plain___file__;
static PyObject *const_str_plain_C;
static PyObject *const_str_plain_im_self;
static PyObject *const_tuple_str_plain_ForkingPickler_tuple;
extern PyObject *const_str_digest_1c9b99ef582c6b5705145380d18cd593;
extern PyObject *const_str_plain___module__;
static PyObject *const_tuple_str_plain_m_tuple;
static PyObject *const_str_digest_95375e0006507f47fc2519eb9b142fac;
static PyObject *const_str_plain__reduce_compiled_method;
static PyObject *const_str_plain_ForkingPickler;
extern PyObject *const_str_plain_f;
extern PyObject *const_str_plain___metaclass__;
static PyObject *module_filename_obj;

/* Indicator if this modules private constants were created yet. */
static bool constants_created = false;

/* Function to create module private constants. */
static void createModuleConstants(void) {
    const_str_plain_register = UNSTREAM_STRING(&constant_bin[ 2630 ], 8, 1);
    const_str_plain_im_func = UNSTREAM_STRING(&constant_bin[ 36453 ], 7, 1);
    const_str_digest_a5e1b9d507c3b6e0344a6503f9256148 = UNSTREAM_STRING(&constant_bin[ 36460 ], 33, 0);
    const_str_plain_im_class = UNSTREAM_STRING(&constant_bin[ 36493 ], 8, 1);
    const_str_digest_83826bb58b51044e5d41a9cc646c18d6 = UNSTREAM_STRING(&constant_bin[ 36501 ], 20, 0);
    const_str_digest_a2691ee6974e7c106bcdaf1f585bded9 = UNSTREAM_STRING(&constant_bin[ 36468 ], 24, 0);
    const_str_plain_C = UNSTREAM_CHAR(67, 1);
    const_str_plain_im_self = UNSTREAM_STRING(&constant_bin[ 36521 ], 7, 1);
    const_tuple_str_plain_ForkingPickler_tuple = PyTuple_New(1);
    const_str_plain_ForkingPickler = UNSTREAM_STRING(&constant_bin[ 4739 ], 14, 1);
    PyTuple_SET_ITEM(const_tuple_str_plain_ForkingPickler_tuple, 0, const_str_plain_ForkingPickler); Py_INCREF(const_str_plain_ForkingPickler);
    const_tuple_str_plain_m_tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(const_tuple_str_plain_m_tuple, 0, const_str_plain_m); Py_INCREF(const_str_plain_m);
    const_str_digest_95375e0006507f47fc2519eb9b142fac = UNSTREAM_STRING(&constant_bin[ 36528 ], 25, 0);
    const_str_plain__reduce_compiled_method = UNSTREAM_STRING(&constant_bin[ 4798 ], 23, 1);

    constants_created = true;
}

/* Function to verify module private constants for non-corruption. */
#ifndef __NUITKA_NO_ASSERT__
void checkModuleConstants_multiprocessing$$45$postLoad(void) {
    // The module may not have been used at all, then ignore this.
    if (constants_created == false) return;


}
#endif

// The module code objects.
static PyCodeObject *codeobj_96ead5d6cba66130403b84e6e9b97e57;
static PyCodeObject *codeobj_1242dbb839dd7fcc07f8eea4c3cbc82a;
static PyCodeObject *codeobj_e145e735688fbf62047959431e595694;

static void createModuleCodeObjects(void) {
    module_filename_obj = MAKE_RELATIVE_PATH(const_str_digest_83826bb58b51044e5d41a9cc646c18d6);
    codeobj_96ead5d6cba66130403b84e6e9b97e57 = MAKE_CODEOBJECT(module_filename_obj, 1, CO_NOFREE, const_str_digest_a5e1b9d507c3b6e0344a6503f9256148, const_tuple_empty, 0, 0, 0);
    codeobj_1242dbb839dd7fcc07f8eea4c3cbc82a = MAKE_CODEOBJECT(module_filename_obj, 10, CO_OPTIMIZED | CO_NEWLOCALS | CO_NOFREE, const_str_plain__reduce_compiled_method, const_tuple_str_plain_m_tuple, 1, 0, 0);
    codeobj_e145e735688fbf62047959431e595694 = MAKE_CODEOBJECT(module_filename_obj, 7, CO_NOFREE, const_str_plain_f, const_tuple_empty, 0, 0, 0);
}

// The module function declarations.
static PyObject *MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_1_f();


static PyObject *MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method();


// The module function definitions.
static PyObject *impl_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method(struct Nuitka_FunctionObject const *self, PyObject **python_pars) {
    // Preserve error status for checks
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
#endif

    // Local variable declarations.
    PyObject *par_m = python_pars[0];
    struct Nuitka_FrameObject *frame_1242dbb839dd7fcc07f8eea4c3cbc82a;
    NUITKA_MAY_BE_UNUSED char const *type_description_1 = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    PyObject *tmp_return_value = NULL;
    static struct Nuitka_FrameObject *cache_frame_1242dbb839dd7fcc07f8eea4c3cbc82a = NULL;

    // Actual function body.
    MAKE_OR_REUSE_FRAME(cache_frame_1242dbb839dd7fcc07f8eea4c3cbc82a, codeobj_1242dbb839dd7fcc07f8eea4c3cbc82a, module_multiprocessing$$45$postLoad, sizeof(void *));
    frame_1242dbb839dd7fcc07f8eea4c3cbc82a = cache_frame_1242dbb839dd7fcc07f8eea4c3cbc82a;

    // Push the new frame as the currently active one.
    pushFrameStack(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);

    // Mark the frame object as in use, ref count 1 will be up for reuse.
    assert(Py_REFCNT(frame_1242dbb839dd7fcc07f8eea4c3cbc82a) == 2); // Frame stack

    // Framed code:
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_source_name_1;
        CHECK_OBJECT(par_m);
        tmp_source_name_1 = par_m;
        tmp_compexpr_left_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain_im_self);
        if (tmp_compexpr_left_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 11;
            type_description_1 = "o";
            goto frame_exception_exit_1;
        }
        tmp_compexpr_right_1 = Py_None;
        tmp_condition_result_1 = (tmp_compexpr_left_1 == tmp_compexpr_right_1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_1);
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        {
            PyObject *tmp_tuple_element_1;
            PyObject *tmp_tuple_element_2;
            PyObject *tmp_source_name_2;
            PyObject *tmp_source_name_3;
            PyObject *tmp_source_name_4;
            tmp_tuple_element_1 = LOOKUP_BUILTIN(const_str_plain_getattr);
            assert(tmp_tuple_element_1 != NULL);
            tmp_return_value = PyTuple_New(2);
            Py_INCREF(tmp_tuple_element_1);
            PyTuple_SET_ITEM(tmp_return_value, 0, tmp_tuple_element_1);
            CHECK_OBJECT(par_m);
            tmp_source_name_2 = par_m;
            tmp_tuple_element_2 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain_im_class);
            if (tmp_tuple_element_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);

                exception_lineno = 12;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            tmp_tuple_element_1 = PyTuple_New(2);
            PyTuple_SET_ITEM(tmp_tuple_element_1, 0, tmp_tuple_element_2);
            CHECK_OBJECT(par_m);
            tmp_source_name_4 = par_m;
            tmp_source_name_3 = LOOKUP_ATTRIBUTE(tmp_source_name_4, const_str_plain_im_func);
            if (tmp_source_name_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);
                Py_DECREF(tmp_tuple_element_1);

                exception_lineno = 12;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            tmp_tuple_element_2 = LOOKUP_ATTRIBUTE(tmp_source_name_3, const_str_plain___name__);
            Py_DECREF(tmp_source_name_3);
            if (tmp_tuple_element_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);
                Py_DECREF(tmp_tuple_element_1);

                exception_lineno = 12;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            PyTuple_SET_ITEM(tmp_tuple_element_1, 1, tmp_tuple_element_2);
            PyTuple_SET_ITEM(tmp_return_value, 1, tmp_tuple_element_1);
            goto frame_return_exit_1;
        }
        goto branch_end_1;
        branch_no_1:;
        {
            PyObject *tmp_tuple_element_3;
            PyObject *tmp_tuple_element_4;
            PyObject *tmp_source_name_5;
            PyObject *tmp_source_name_6;
            PyObject *tmp_source_name_7;
            tmp_tuple_element_3 = LOOKUP_BUILTIN(const_str_plain_getattr);
            assert(tmp_tuple_element_3 != NULL);
            tmp_return_value = PyTuple_New(2);
            Py_INCREF(tmp_tuple_element_3);
            PyTuple_SET_ITEM(tmp_return_value, 0, tmp_tuple_element_3);
            CHECK_OBJECT(par_m);
            tmp_source_name_5 = par_m;
            tmp_tuple_element_4 = LOOKUP_ATTRIBUTE(tmp_source_name_5, const_str_plain_im_self);
            if (tmp_tuple_element_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);

                exception_lineno = 14;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            tmp_tuple_element_3 = PyTuple_New(2);
            PyTuple_SET_ITEM(tmp_tuple_element_3, 0, tmp_tuple_element_4);
            CHECK_OBJECT(par_m);
            tmp_source_name_7 = par_m;
            tmp_source_name_6 = LOOKUP_ATTRIBUTE(tmp_source_name_7, const_str_plain_im_func);
            if (tmp_source_name_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);
                Py_DECREF(tmp_tuple_element_3);

                exception_lineno = 14;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            tmp_tuple_element_4 = LOOKUP_ATTRIBUTE(tmp_source_name_6, const_str_plain___name__);
            Py_DECREF(tmp_source_name_6);
            if (tmp_tuple_element_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_return_value);
                Py_DECREF(tmp_tuple_element_3);

                exception_lineno = 14;
                type_description_1 = "o";
                goto frame_exception_exit_1;
            }
            PyTuple_SET_ITEM(tmp_tuple_element_3, 1, tmp_tuple_element_4);
            PyTuple_SET_ITEM(tmp_return_value, 1, tmp_tuple_element_3);
            goto frame_return_exit_1;
        }
        branch_end_1:;
    }

#if 0
    RESTORE_FRAME_EXCEPTION(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);
#endif

    // Put the previous frame back on top.
    popFrameStack();

    goto frame_no_exception_1;

    frame_return_exit_1:;
#if 0
    RESTORE_FRAME_EXCEPTION(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);
#endif

    // Put the previous frame back on top.
    popFrameStack();

    goto function_return_exit;

    frame_exception_exit_1:;

#if 0
    RESTORE_FRAME_EXCEPTION(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);
#endif

    if (exception_tb == NULL) {
        exception_tb = MAKE_TRACEBACK(frame_1242dbb839dd7fcc07f8eea4c3cbc82a, exception_lineno);
    }
    else if (exception_tb->tb_frame != &frame_1242dbb839dd7fcc07f8eea4c3cbc82a->m_frame) {
        exception_tb = ADD_TRACEBACK(exception_tb, frame_1242dbb839dd7fcc07f8eea4c3cbc82a, exception_lineno);
    }

    // Attachs locals to frame if any.
    Nuitka_Frame_AttachLocals(
        (struct Nuitka_FrameObject *)frame_1242dbb839dd7fcc07f8eea4c3cbc82a,
        type_description_1,
        par_m
    );


    // Release cached frame.
    if (frame_1242dbb839dd7fcc07f8eea4c3cbc82a == cache_frame_1242dbb839dd7fcc07f8eea4c3cbc82a) {
        Py_DECREF(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);
    }
    cache_frame_1242dbb839dd7fcc07f8eea4c3cbc82a = NULL;

    assertFrameObject(frame_1242dbb839dd7fcc07f8eea4c3cbc82a);

    // Put the previous frame back on top.
    popFrameStack();

    // Return the error.
    goto function_exception_exit;

    frame_no_exception_1:;

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_m);
    Py_DECREF(par_m);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_m);
    Py_DECREF(par_m);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}



static PyObject *MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_1_f() {
    struct Nuitka_FunctionObject *result = Nuitka_Function_New(
        NULL,
        const_str_plain_f,
#if PYTHON_VERSION >= 300
        NULL,
#endif
        codeobj_e145e735688fbf62047959431e595694,
        NULL,
#if PYTHON_VERSION >= 300
        NULL,
        NULL,
#endif
        module_multiprocessing$$45$postLoad,
        NULL,
        0
    );

    return (PyObject *)result;
}



static PyObject *MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method() {
    struct Nuitka_FunctionObject *result = Nuitka_Function_New(
        impl_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method,
        const_str_plain__reduce_compiled_method,
#if PYTHON_VERSION >= 300
        NULL,
#endif
        codeobj_1242dbb839dd7fcc07f8eea4c3cbc82a,
        NULL,
#if PYTHON_VERSION >= 300
        NULL,
        NULL,
#endif
        module_multiprocessing$$45$postLoad,
        NULL,
        0
    );

    return (PyObject *)result;
}



#if PYTHON_VERSION >= 300
static struct PyModuleDef mdef_multiprocessing$$45$postLoad =
{
    PyModuleDef_HEAD_INIT,
    NULL,                /* m_name, filled later */
    NULL,                /* m_doc */
    -1,                  /* m_size */
    NULL,                /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
  };
#endif

extern PyObject *const_str_plain___compiled__;

extern PyObject *const_str_plain___package__;
extern PyObject *const_str_empty;

#if PYTHON_VERSION >= 300
extern PyObject *const_str_dot;
extern PyObject *const_str_plain___loader__;
#endif

#if PYTHON_VERSION >= 340
extern PyObject *const_str_plain___spec__;
extern PyObject *const_str_plain__initializing;
extern PyObject *const_str_plain_submodule_search_locations;
#endif

extern void _initCompiledCellType();
extern void _initCompiledGeneratorType();
extern void _initCompiledFunctionType();
extern void _initCompiledMethodType();
extern void _initCompiledFrameType();
#if PYTHON_VERSION >= 350
extern void _initCompiledCoroutineTypes();
#endif
#if PYTHON_VERSION >= 360
extern void _initCompiledAsyncgenTypes();
#endif

extern PyTypeObject Nuitka_Loader_Type;

#ifdef _NUITKA_PLUGIN_DILL_ENABLED
// Provide a way to create find a function via its C code and create it back
// in another process, useful for multiprocessing extensions like dill

function_impl_code functable_multiprocessing$$45$postLoad[] = {
    NULL,
    impl_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method,
    NULL
};

static char const *_reduce_compiled_function_argnames[] = {
    "func",
    NULL
};

static PyObject *_reduce_compiled_function(PyObject *self, PyObject *args, PyObject *kwds) {
    PyObject *func;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:reduce_compiled_function", (char **)_reduce_compiled_function_argnames, &func, NULL)) {
        return NULL;
    }

    if (Nuitka_Function_Check(func) == false) {
        PyErr_Format(PyExc_TypeError, "not a compiled function");
        return NULL;
    }

    struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)func;

    function_impl_code *current = functable_multiprocessing$$45$postLoad;
    int offset = 0;

    while (*current != NULL) {
        if (*current == function->m_c_code) {
            break;
        }

        current += 1;
        offset += 1;
    }

    if (*current == NULL) {
        PyErr_Format(PyExc_TypeError, "Cannot find compiled function in module.");
        return NULL;
    }

    PyObject *code_object_desc = PyTuple_New(6);
    PyTuple_SET_ITEM0(code_object_desc, 0, function->m_code_object->co_filename);
    PyTuple_SET_ITEM0(code_object_desc, 1, function->m_code_object->co_name);
    PyTuple_SET_ITEM(code_object_desc, 2, PyLong_FromLong(function->m_code_object->co_firstlineno));
    PyTuple_SET_ITEM0(code_object_desc, 3, function->m_code_object->co_varnames);
    PyTuple_SET_ITEM(code_object_desc, 4, PyLong_FromLong(function->m_code_object->co_argcount));
    PyTuple_SET_ITEM(code_object_desc, 5, PyLong_FromLong(function->m_code_object->co_flags));

    CHECK_OBJECT_DEEP(code_object_desc);

    PyObject *result = PyTuple_New(4);
    PyTuple_SET_ITEM(result, 0, PyLong_FromLong(offset));
    PyTuple_SET_ITEM(result, 1, code_object_desc);
    PyTuple_SET_ITEM0(result, 2, function->m_defaults);
    PyTuple_SET_ITEM0(result, 3, function->m_doc != NULL ? function->m_doc : Py_None);

    CHECK_OBJECT_DEEP(result);

    return result;
}

static PyMethodDef _method_def_reduce_compiled_function = {"reduce_compiled_function", (PyCFunction)_reduce_compiled_function,
                                                           METH_VARARGS | METH_KEYWORDS, NULL};

static char const *_create_compiled_function_argnames[] = {
    "func",
    "code_object_desc",
    "defaults",
    "doc",
    NULL
};


static PyObject *_create_compiled_function(PyObject *self, PyObject *args, PyObject *kwds) {
    CHECK_OBJECT_DEEP(args);

    PyObject *func;
    PyObject *code_object_desc;
    PyObject *defaults;
    PyObject *doc;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOOO:create_compiled_function", (char **)_create_compiled_function_argnames, &func, &code_object_desc, &defaults, &doc, NULL)) {
        return NULL;
    }

    int offset = PyLong_AsLong(func);

    if (offset == -1 && ERROR_OCCURRED()) {
        return NULL;
    }

    if (offset > sizeof(functable_multiprocessing$$45$postLoad) || offset < 0) {
        PyErr_Format(PyExc_TypeError, "Wrong offset for compiled function.");
        return NULL;
    }

    PyObject *filename = PyTuple_GET_ITEM(code_object_desc, 0);
    PyObject *function_name = PyTuple_GET_ITEM(code_object_desc, 1);
    PyObject *line = PyTuple_GET_ITEM(code_object_desc, 2);
    int line_int = PyLong_AsLong(line);
    assert(!ERROR_OCCURRED());

    PyObject *argnames = PyTuple_GET_ITEM(code_object_desc, 3);
    PyObject *arg_count = PyTuple_GET_ITEM(code_object_desc, 4);
    int arg_count_int = PyLong_AsLong(arg_count);
    assert(!ERROR_OCCURRED());
    PyObject *flags = PyTuple_GET_ITEM(code_object_desc, 5);
    int flags_int = PyLong_AsLong(flags);
    assert(!ERROR_OCCURRED());

    PyCodeObject *code_object = MAKE_CODEOBJECT(
        filename,
        line_int,
        flags_int,
        function_name,
        argnames,
        arg_count_int,
        0, // TODO: Missing kw_only_count
        0 // TODO: Missing pos_only_count
    );

    // TODO: More stuff needed for Python3, best to re-order arguments of MAKE_CODEOBJECT.
    struct Nuitka_FunctionObject *result = Nuitka_Function_New(
        functable_multiprocessing$$45$postLoad[offset],
        code_object->co_name,
#if PYTHON_VERSION >= 300
        NULL, // TODO: Not transferring qualname yet
#endif
        code_object,
        defaults,
#if PYTHON_VERSION >= 300
        NULL, // kwdefaults are done on the outside currently
        NULL, // TODO: Not transferring annotations
#endif
        module_multiprocessing$$45$postLoad,
        doc,
        0
    );

    return (PyObject *)result;
}

static PyMethodDef _method_def_create_compiled_function = {
    "create_compiled_function",
    (PyCFunction)_create_compiled_function,
    METH_VARARGS | METH_KEYWORDS, NULL
};


#endif

// Internal entry point for module code.
PyObject *modulecode_multiprocessing$$45$postLoad(char const *module_full_name) {
#if defined(_NUITKA_EXE) || PYTHON_VERSION >= 300
    static bool _init_done = false;

    // Modules might be imported repeatedly, which is to be ignored.
    if (_init_done) {
        return module_multiprocessing$$45$postLoad;
    } else {
        _init_done = true;
    }
#endif

#ifdef _NUITKA_MODULE
    // In case of a stand alone extension module, need to call initialization
    // the init here because that's the first and only time we are going to get
    // called here.

    // May have to activate constants blob.
#if defined(_NUITKA_CONSTANTS_FROM_RESOURCE)
    loadConstantsResource();
#endif

    // Initialize the constant values used.
    _initBuiltinModule();
    createGlobalConstants();

    /* Initialize the compiled types of Nuitka. */
    _initCompiledCellType();
    _initCompiledGeneratorType();
    _initCompiledFunctionType();
    _initCompiledMethodType();
    _initCompiledFrameType();
#if PYTHON_VERSION >= 350
    _initCompiledCoroutineTypes();
#endif
#if PYTHON_VERSION >= 360
    _initCompiledAsyncgenTypes();
#endif

#if PYTHON_VERSION < 300
    _initSlotCompare();
#endif
#if PYTHON_VERSION >= 270
    _initSlotIternext();
#endif

    patchBuiltinModule();
    patchTypeComparison();

    // Enable meta path based loader if not already done.
#ifdef _NUITKA_TRACE
    PRINT_STRING("multiprocessing-postLoad: Calling setupMetaPathBasedLoader().\n");
#endif
    setupMetaPathBasedLoader();

#if PYTHON_VERSION >= 300
    patchInspectModule();
#endif

#endif

    /* The constants only used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("multiprocessing-postLoad: Calling createModuleConstants().\n");
#endif
    createModuleConstants();

    /* The code objects used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("multiprocessing-postLoad: Calling createModuleCodeObjects().\n");
#endif
    createModuleCodeObjects();

    // PRINT_STRING("in initmultiprocessing$$45$postLoad\n");

    // Create the module object first. There are no methods initially, all are
    // added dynamically in actual code only.  Also no "__doc__" is initially
    // set at this time, as it could not contain NUL characters this way, they
    // are instead set in early module code.  No "self" for modules, we have no
    // use for it.
#if PYTHON_VERSION < 300
    module_multiprocessing$$45$postLoad = Py_InitModule4(
        module_full_name,        // Module Name
        NULL,                    // No methods initially, all are added
                                 // dynamically in actual module code only.
        NULL,                    // No "__doc__" is initially set, as it could
                                 // not contain NUL this way, added early in
                                 // actual code.
        NULL,                    // No self for modules, we don't use it.
        PYTHON_API_VERSION
    );
#else
    mdef_multiprocessing$$45$postLoad.m_name = module_full_name;
    module_multiprocessing$$45$postLoad = PyModule_Create(&mdef_multiprocessing$$45$postLoad);
#endif

    moduledict_multiprocessing$$45$postLoad = MODULE_DICT(module_multiprocessing$$45$postLoad);

#ifdef _NUITKA_PLUGIN_DILL_ENABLED
    {
        PyObject *function_tables = PyObject_GetAttrString((PyObject *)builtin_module, "compiled_function_tables");
        if (function_tables == NULL)
        {
            DROP_ERROR_OCCURRED();
            function_tables = PyDict_New();
        }
        PyObject_SetAttrString((PyObject *)builtin_module, "compiled_function_tables", function_tables);
        PyObject *funcs = PyTuple_New(2);
        PyTuple_SetItem(funcs, 0, PyCFunction_New(&_method_def_reduce_compiled_function, NULL));
        PyTuple_SetItem(funcs, 1, PyCFunction_New(&_method_def_create_compiled_function, NULL));
        PyDict_SetItemString(function_tables, module_full_name, funcs);
    }
#endif

    // Set "__compiled__" to what version information we have.
    UPDATE_STRING_DICT0(
        moduledict_multiprocessing$$45$postLoad,
        (Nuitka_StringObject *)const_str_plain___compiled__,
        Nuitka_dunder_compiled_value
    );

    // Update "__package__" value to what it ought to be.
    {
#if 0
        UPDATE_STRING_DICT0(
            moduledict_multiprocessing$$45$postLoad,
            (Nuitka_StringObject *)const_str_plain___package__,
            const_str_empty
        );
#elif 0
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___name__);

        UPDATE_STRING_DICT0(
            moduledict_multiprocessing$$45$postLoad,
            (Nuitka_StringObject *)const_str_plain___package__,
            module_name
        );
#else

#if PYTHON_VERSION < 300
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___name__);
        char const *module_name_cstr = PyString_AS_STRING(module_name);

        char const *last_dot = strrchr(module_name_cstr, '.');

        if (last_dot != NULL)
        {
            UPDATE_STRING_DICT1(
                moduledict_multiprocessing$$45$postLoad,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyString_FromStringAndSize(module_name_cstr, last_dot - module_name_cstr)
            );
        }
#else
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___name__);
        Py_ssize_t dot_index = PyUnicode_Find(module_name, const_str_dot, 0, PyUnicode_GetLength(module_name), -1);

        if (dot_index != -1)
        {
            UPDATE_STRING_DICT1(
                moduledict_multiprocessing$$45$postLoad,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyUnicode_Substring(module_name, 0, dot_index)
            );
        }
#endif
#endif
    }

    CHECK_OBJECT(module_multiprocessing$$45$postLoad);

// Seems to work for Python2.7 out of the box, but for Python3, the module
// doesn't automatically enter "sys.modules", so do it manually.
#if PYTHON_VERSION >= 300
    {
        int r = PyDict_SetItemString(PyImport_GetModuleDict(), module_full_name, module_multiprocessing$$45$postLoad);
        assert(r != -1);
    }
#endif

    // For deep importing of a module we need to have "__builtins__", so we set
    // it ourselves in the same way than CPython does. Note: This must be done
    // before the frame object is allocated, or else it may fail.

    if (GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___builtins__) == NULL)
    {
        PyObject *value = (PyObject *)builtin_module;

        // Check if main module, not a dict then but the module itself.
#if !defined(_NUITKA_EXE) || !0
        value = PyModule_GetDict(value);
#endif

        UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___builtins__, value);
    }

#if PYTHON_VERSION >= 300
    UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___loader__, (PyObject *)&Nuitka_Loader_Type);
#endif

#if PYTHON_VERSION >= 340
// Set the "__spec__" value

#if 0
    // Main modules just get "None" as spec.
    UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___spec__, Py_None);
#else
    // Other modules get a "ModuleSpec" from the standard mechanism.
    {
        PyObject *bootstrap_module = PyImport_ImportModule("importlib._bootstrap");
        CHECK_OBJECT(bootstrap_module);
        PyObject *module_spec_class = PyObject_GetAttrString(bootstrap_module, "ModuleSpec");
        Py_DECREF(bootstrap_module);

        PyObject *args[] = {
            GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___name__),
            (PyObject *)&Nuitka_Loader_Type
        };

        PyObject *spec_value = CALL_FUNCTION_WITH_ARGS2(
            module_spec_class,
            args
        );
        Py_DECREF(module_spec_class);

        // We can assume this to never fail, or else we are in trouble anyway.
        CHECK_OBJECT(spec_value);

// For packages set the submodule search locations as well, even if to empty
// list, so investigating code will consider it a package.
#if 0
        SET_ATTRIBUTE(spec_value, const_str_plain_submodule_search_locations, PyList_New(0));
#endif

// Mark the execution in the "__spec__" value.
        SET_ATTRIBUTE(spec_value, const_str_plain__initializing, Py_True);

        UPDATE_STRING_DICT1(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___spec__, spec_value);
    }
#endif
#endif

    // Temp variables if any
    PyObject *tmp_class_creation_1__class = NULL;
    PyObject *tmp_class_creation_1__class_dict = NULL;
    PyObject *tmp_class_creation_1__metaclass = NULL;
    PyObject *tmp_locals_multiprocessing$$45$postLoad_6_key_f = NULL;
    struct Nuitka_FrameObject *frame_96ead5d6cba66130403b84e6e9b97e57;
    NUITKA_MAY_BE_UNUSED char const *type_description_1 = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;
    int tmp_res;
    bool tmp_result;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
    PyObject *exception_keeper_type_3;
    PyObject *exception_keeper_value_3;
    PyTracebackObject *exception_keeper_tb_3;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_3;

    // Module code.
    {
        PyObject *tmp_assign_source_1;
        tmp_assign_source_1 = Py_None;
        UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___doc__, tmp_assign_source_1);
    }
    {
        PyObject *tmp_assign_source_2;
        tmp_assign_source_2 = module_filename_obj;
        UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___file__, tmp_assign_source_2);
    }
    // Frame without reuse.
    frame_96ead5d6cba66130403b84e6e9b97e57 = MAKE_MODULE_FRAME(codeobj_96ead5d6cba66130403b84e6e9b97e57, module_multiprocessing$$45$postLoad);

    // Push the new frame as the currently active one, and we should be exclusively
    // owning it.
    pushFrameStack(frame_96ead5d6cba66130403b84e6e9b97e57);
    assert(Py_REFCNT(frame_96ead5d6cba66130403b84e6e9b97e57) == 2);

    // Framed code:
    // Tried code:
    {
        PyObject *tmp_assign_source_3;
        PyObject *tmp_import_name_from_1;
        PyObject *tmp_name_name_1;
        PyObject *tmp_globals_name_1;
        PyObject *tmp_locals_name_1;
        PyObject *tmp_fromlist_name_1;
        tmp_name_name_1 = const_str_digest_1c9b99ef582c6b5705145380d18cd593;
        tmp_globals_name_1 = (PyObject *)moduledict_multiprocessing$$45$postLoad;
        tmp_locals_name_1 = Py_None;
        tmp_fromlist_name_1 = const_tuple_str_plain_ForkingPickler_tuple;
        frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 2;
        tmp_import_name_from_1 = IMPORT_MODULE4(tmp_name_name_1, tmp_globals_name_1, tmp_locals_name_1, tmp_fromlist_name_1);
        if (tmp_import_name_from_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 2;

            goto try_except_handler_1;
        }
        tmp_assign_source_3 = IMPORT_NAME(tmp_import_name_from_1, const_str_plain_ForkingPickler);
        Py_DECREF(tmp_import_name_from_1);
        if (tmp_assign_source_3 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 2;

            goto try_except_handler_1;
        }
        UPDATE_STRING_DICT1(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_ForkingPickler, tmp_assign_source_3);
    }
    goto try_end_1;
    // Exception handler code:
    try_except_handler_1:;
    exception_keeper_type_1 = exception_type;
    exception_keeper_value_1 = exception_value;
    exception_keeper_tb_1 = exception_tb;
    exception_keeper_lineno_1 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    // Preserve existing published exception.
    PRESERVE_FRAME_EXCEPTION(frame_96ead5d6cba66130403b84e6e9b97e57);
    if (exception_keeper_tb_1 == NULL) {
        exception_keeper_tb_1 = MAKE_TRACEBACK(frame_96ead5d6cba66130403b84e6e9b97e57, exception_keeper_lineno_1);
    } else if (exception_keeper_lineno_1 != 0) {
        exception_keeper_tb_1 = ADD_TRACEBACK(exception_keeper_tb_1, frame_96ead5d6cba66130403b84e6e9b97e57, exception_keeper_lineno_1);
    }

    NORMALIZE_EXCEPTION(&exception_keeper_type_1, &exception_keeper_value_1, &exception_keeper_tb_1);
    PUBLISH_EXCEPTION(&exception_keeper_type_1, &exception_keeper_value_1, &exception_keeper_tb_1);
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        tmp_compexpr_left_1 = EXC_TYPE(PyThreadState_GET());
        tmp_compexpr_right_1 = PyExc_ImportError;
        tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_1, tmp_compexpr_right_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 3;

            goto frame_exception_exit_1;
        }
        tmp_condition_result_1 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        {
            PyObject *tmp_assign_source_4;
            PyObject *tmp_import_name_from_2;
            PyObject *tmp_name_name_2;
            PyObject *tmp_globals_name_2;
            PyObject *tmp_locals_name_2;
            PyObject *tmp_fromlist_name_2;
            tmp_name_name_2 = const_str_digest_95375e0006507f47fc2519eb9b142fac;
            tmp_globals_name_2 = (PyObject *)moduledict_multiprocessing$$45$postLoad;
            tmp_locals_name_2 = Py_None;
            tmp_fromlist_name_2 = const_tuple_str_plain_ForkingPickler_tuple;
            frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 4;
            tmp_import_name_from_2 = IMPORT_MODULE4(tmp_name_name_2, tmp_globals_name_2, tmp_locals_name_2, tmp_fromlist_name_2);
            if (tmp_import_name_from_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 4;

                goto frame_exception_exit_1;
            }
            tmp_assign_source_4 = IMPORT_NAME(tmp_import_name_from_2, const_str_plain_ForkingPickler);
            Py_DECREF(tmp_import_name_from_2);
            if (tmp_assign_source_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 4;

                goto frame_exception_exit_1;
            }
            UPDATE_STRING_DICT1(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_ForkingPickler, tmp_assign_source_4);
        }
        goto branch_end_1;
        branch_no_1:;
        tmp_result = RERAISE_EXCEPTION(&exception_type, &exception_value, &exception_tb);
        if (unlikely(tmp_result == false)) {
            exception_lineno = 1;
        }

        if (exception_tb && exception_tb->tb_frame == &frame_96ead5d6cba66130403b84e6e9b97e57->m_frame) frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = exception_tb->tb_lineno;

        goto frame_exception_exit_1;
        branch_end_1:;
    }
    goto try_end_1;
    // exception handler codes exits in all cases
    NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad);
    return NULL;
    // End of try:
    try_end_1:;
    {
        PyObject *tmp_assign_source_5;
        {
            PyObject *tmp_assign_source_6;
            tmp_assign_source_6 = MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_1_f();



            assert(tmp_locals_multiprocessing$$45$postLoad_6_key_f == NULL);
            tmp_locals_multiprocessing$$45$postLoad_6_key_f = tmp_assign_source_6;
        }
        // Tried code:
        {
            PyObject *tmp_dict_key_1;
            PyObject *tmp_dict_value_1;
            PyObject *tmp_dict_key_2;
            PyObject *tmp_dict_value_2;
            tmp_dict_value_1 = const_str_digest_a2691ee6974e7c106bcdaf1f585bded9;
            tmp_dict_key_1 = const_str_plain___module__;
            tmp_assign_source_5 = _PyDict_NewPresized( 2 );
            tmp_res = PyDict_SetItem(tmp_assign_source_5, tmp_dict_key_1, tmp_dict_value_1);
            assert(!(tmp_res != 0));
            CHECK_OBJECT(tmp_locals_multiprocessing$$45$postLoad_6_key_f);
            tmp_dict_value_2 = tmp_locals_multiprocessing$$45$postLoad_6_key_f;
            tmp_dict_key_2 = const_str_plain_f;
            tmp_res = PyDict_SetItem(tmp_assign_source_5, tmp_dict_key_2, tmp_dict_value_2);
            assert(!(tmp_res != 0));
            goto try_return_handler_2;
        }
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad);
        return NULL;
        // Return handler code:
        try_return_handler_2:;
        CHECK_OBJECT((PyObject *)tmp_locals_multiprocessing$$45$postLoad_6_key_f);
        Py_DECREF(tmp_locals_multiprocessing$$45$postLoad_6_key_f);
        tmp_locals_multiprocessing$$45$postLoad_6_key_f = NULL;

        goto outline_result_1;
        // End of try:
        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad);
        return NULL;
        outline_result_1:;
        assert(tmp_class_creation_1__class_dict == NULL);
        tmp_class_creation_1__class_dict = tmp_assign_source_5;
    }
    // Tried code:
    {
        PyObject *tmp_assign_source_7;
        nuitka_bool tmp_condition_result_2;
        PyObject *tmp_key_name_1;
        PyObject *tmp_dict_name_1;
        PyObject *tmp_dict_name_2;
        PyObject *tmp_key_name_2;
        tmp_key_name_1 = const_str_plain___metaclass__;
        CHECK_OBJECT(tmp_class_creation_1__class_dict);
        tmp_dict_name_1 = tmp_class_creation_1__class_dict;
        tmp_res = PyDict_Contains(tmp_dict_name_1, tmp_key_name_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 6;

            goto try_except_handler_3;
        }
        tmp_condition_result_2 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_2 == NUITKA_BOOL_TRUE) {
            goto condexpr_true_1;
        } else {
            goto condexpr_false_1;
        }
        condexpr_true_1:;
        CHECK_OBJECT(tmp_class_creation_1__class_dict);
        tmp_dict_name_2 = tmp_class_creation_1__class_dict;
        tmp_key_name_2 = const_str_plain___metaclass__;
        tmp_assign_source_7 = DICT_GET_ITEM(tmp_dict_name_2, tmp_key_name_2);
        if (tmp_assign_source_7 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 6;

            goto try_except_handler_3;
        }
        goto condexpr_end_1;
        condexpr_false_1:;
        // Tried code:
        {
            PyObject *tmp_mvar_value_1;
            tmp_mvar_value_1 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain___metaclass__);

            if (unlikely(tmp_mvar_value_1 == NULL)) {
                tmp_mvar_value_1 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain___metaclass__);
            }

            if (tmp_mvar_value_1 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 932 ], 35, 0);
                exception_tb = NULL;

                exception_lineno = 6;

                goto try_except_handler_4;
            }

            tmp_assign_source_7 = tmp_mvar_value_1;
            Py_INCREF(tmp_assign_source_7);
            goto outline_result_2;
        }
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad);
        return NULL;
        // Exception handler code:
        try_except_handler_4:;
        exception_keeper_type_2 = exception_type;
        exception_keeper_value_2 = exception_value;
        exception_keeper_tb_2 = exception_tb;
        exception_keeper_lineno_2 = exception_lineno;
        exception_type = NULL;
        exception_value = NULL;
        exception_tb = NULL;
        exception_lineno = 0;

        Py_DECREF(exception_keeper_type_2);
        Py_XDECREF(exception_keeper_value_2);
        Py_XDECREF(exception_keeper_tb_2);
        tmp_assign_source_7 = (PyObject *)&PyClass_Type;
        Py_INCREF(tmp_assign_source_7);
        goto outline_result_2;
        // End of try:
        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(multiprocessing$$45$postLoad);
        return NULL;
        outline_result_2:;
        condexpr_end_1:;
        assert(tmp_class_creation_1__metaclass == NULL);
        tmp_class_creation_1__metaclass = tmp_assign_source_7;
    }
    {
        PyObject *tmp_assign_source_8;
        PyObject *tmp_called_name_1;
        PyObject *tmp_args_element_name_1;
        PyObject *tmp_args_element_name_2;
        PyObject *tmp_args_element_name_3;
        CHECK_OBJECT(tmp_class_creation_1__metaclass);
        tmp_called_name_1 = tmp_class_creation_1__metaclass;
        tmp_args_element_name_1 = const_str_plain_C;
        tmp_args_element_name_2 = const_tuple_empty;
        CHECK_OBJECT(tmp_class_creation_1__class_dict);
        tmp_args_element_name_3 = tmp_class_creation_1__class_dict;
        frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 6;
        {
            PyObject *call_args[] = {tmp_args_element_name_1, tmp_args_element_name_2, tmp_args_element_name_3};
            tmp_assign_source_8 = CALL_FUNCTION_WITH_ARGS3(tmp_called_name_1, call_args);
        }

        if (tmp_assign_source_8 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 6;

            goto try_except_handler_3;
        }
        assert(tmp_class_creation_1__class == NULL);
        tmp_class_creation_1__class = tmp_assign_source_8;
    }
    goto try_end_2;
    // Exception handler code:
    try_except_handler_3:;
    exception_keeper_type_3 = exception_type;
    exception_keeper_value_3 = exception_value;
    exception_keeper_tb_3 = exception_tb;
    exception_keeper_lineno_3 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__class_dict);
    Py_DECREF(tmp_class_creation_1__class_dict);
    tmp_class_creation_1__class_dict = NULL;

    Py_XDECREF(tmp_class_creation_1__metaclass);
    tmp_class_creation_1__metaclass = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_3;
    exception_value = exception_keeper_value_3;
    exception_tb = exception_keeper_tb_3;
    exception_lineno = exception_keeper_lineno_3;

    goto frame_exception_exit_1;
    // End of try:
    try_end_2:;
    {
        PyObject *tmp_assign_source_9;
        CHECK_OBJECT(tmp_class_creation_1__class);
        tmp_assign_source_9 = tmp_class_creation_1__class;
        UPDATE_STRING_DICT0(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_C, tmp_assign_source_9);
    }
    CHECK_OBJECT((PyObject *)tmp_class_creation_1__class);
    Py_DECREF(tmp_class_creation_1__class);
    tmp_class_creation_1__class = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__class_dict);
    Py_DECREF(tmp_class_creation_1__class_dict);
    tmp_class_creation_1__class_dict = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__metaclass);
    Py_DECREF(tmp_class_creation_1__metaclass);
    tmp_class_creation_1__metaclass = NULL;

    {
        PyObject *tmp_assign_source_10;
        tmp_assign_source_10 = MAKE_FUNCTION_multiprocessing$$45$postLoad$$$function_2__reduce_compiled_method();



        UPDATE_STRING_DICT1(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain__reduce_compiled_method, tmp_assign_source_10);
    }
    {
        PyObject *tmp_called_name_2;
        PyObject *tmp_source_name_1;
        PyObject *tmp_mvar_value_2;
        PyObject *tmp_call_result_1;
        PyObject *tmp_args_element_name_4;
        PyObject *tmp_type_arg_1;
        PyObject *tmp_source_name_2;
        PyObject *tmp_called_name_3;
        PyObject *tmp_mvar_value_3;
        PyObject *tmp_args_element_name_5;
        PyObject *tmp_mvar_value_4;
        tmp_mvar_value_2 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_ForkingPickler);

        if (unlikely(tmp_mvar_value_2 == NULL)) {
            tmp_mvar_value_2 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_ForkingPickler);
        }

        if (tmp_mvar_value_2 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4733 ], 36, 0);
            exception_tb = NULL;

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }

        tmp_source_name_1 = tmp_mvar_value_2;
        tmp_called_name_2 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain_register);
        if (tmp_called_name_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 16;

            goto frame_exception_exit_1;
        }
        tmp_mvar_value_3 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_C);

        if (unlikely(tmp_mvar_value_3 == NULL)) {
            tmp_mvar_value_3 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_C);
        }

        if (tmp_mvar_value_3 == NULL) {
            Py_DECREF(tmp_called_name_2);
            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4769 ], 23, 0);
            exception_tb = NULL;

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }

        tmp_called_name_3 = tmp_mvar_value_3;
        frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 16;
        tmp_source_name_2 = CALL_FUNCTION_NO_ARGS(tmp_called_name_3);
        if (tmp_source_name_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_2);

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }
        tmp_type_arg_1 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain_f);
        Py_DECREF(tmp_source_name_2);
        if (tmp_type_arg_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_2);

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }
        tmp_args_element_name_4 = BUILTIN_TYPE1(tmp_type_arg_1);
        Py_DECREF(tmp_type_arg_1);
        if (tmp_args_element_name_4 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_2);

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }
        tmp_mvar_value_4 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain__reduce_compiled_method);

        if (unlikely(tmp_mvar_value_4 == NULL)) {
            tmp_mvar_value_4 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__reduce_compiled_method);
        }

        if (tmp_mvar_value_4 == NULL) {
            Py_DECREF(tmp_called_name_2);
            Py_DECREF(tmp_args_element_name_4);
            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4792 ], 45, 0);
            exception_tb = NULL;

            exception_lineno = 16;

            goto frame_exception_exit_1;
        }

        tmp_args_element_name_5 = tmp_mvar_value_4;
        frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 16;
        {
            PyObject *call_args[] = {tmp_args_element_name_4, tmp_args_element_name_5};
            tmp_call_result_1 = CALL_FUNCTION_WITH_ARGS2(tmp_called_name_2, call_args);
        }

        Py_DECREF(tmp_called_name_2);
        Py_DECREF(tmp_args_element_name_4);
        if (tmp_call_result_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 16;

            goto frame_exception_exit_1;
        }
        Py_DECREF(tmp_call_result_1);
    }
    {
        PyObject *tmp_called_name_4;
        PyObject *tmp_source_name_3;
        PyObject *tmp_mvar_value_5;
        PyObject *tmp_call_result_2;
        PyObject *tmp_args_element_name_6;
        PyObject *tmp_type_arg_2;
        PyObject *tmp_source_name_4;
        PyObject *tmp_mvar_value_6;
        PyObject *tmp_args_element_name_7;
        PyObject *tmp_mvar_value_7;
        tmp_mvar_value_5 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_ForkingPickler);

        if (unlikely(tmp_mvar_value_5 == NULL)) {
            tmp_mvar_value_5 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_ForkingPickler);
        }

        if (tmp_mvar_value_5 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4733 ], 36, 0);
            exception_tb = NULL;

            exception_lineno = 18;

            goto frame_exception_exit_1;
        }

        tmp_source_name_3 = tmp_mvar_value_5;
        tmp_called_name_4 = LOOKUP_ATTRIBUTE(tmp_source_name_3, const_str_plain_register);
        if (tmp_called_name_4 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 18;

            goto frame_exception_exit_1;
        }
        tmp_mvar_value_6 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain_C);

        if (unlikely(tmp_mvar_value_6 == NULL)) {
            tmp_mvar_value_6 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_C);
        }

        if (tmp_mvar_value_6 == NULL) {
            Py_DECREF(tmp_called_name_4);
            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4769 ], 23, 0);
            exception_tb = NULL;

            exception_lineno = 18;

            goto frame_exception_exit_1;
        }

        tmp_source_name_4 = tmp_mvar_value_6;
        tmp_type_arg_2 = LOOKUP_ATTRIBUTE(tmp_source_name_4, const_str_plain_f);
        if (tmp_type_arg_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_4);

            exception_lineno = 18;

            goto frame_exception_exit_1;
        }
        tmp_args_element_name_6 = BUILTIN_TYPE1(tmp_type_arg_2);
        Py_DECREF(tmp_type_arg_2);
        if (tmp_args_element_name_6 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_4);

            exception_lineno = 18;

            goto frame_exception_exit_1;
        }
        tmp_mvar_value_7 = GET_STRING_DICT_VALUE(moduledict_multiprocessing$$45$postLoad, (Nuitka_StringObject *)const_str_plain__reduce_compiled_method);

        if (unlikely(tmp_mvar_value_7 == NULL)) {
            tmp_mvar_value_7 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__reduce_compiled_method);
        }

        if (tmp_mvar_value_7 == NULL) {
            Py_DECREF(tmp_called_name_4);
            Py_DECREF(tmp_args_element_name_6);
            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 4792 ], 45, 0);
            exception_tb = NULL;

            exception_lineno = 18;

            goto frame_exception_exit_1;
        }

        tmp_args_element_name_7 = tmp_mvar_value_7;
        frame_96ead5d6cba66130403b84e6e9b97e57->m_frame.f_lineno = 18;
        {
            PyObject *call_args[] = {tmp_args_element_name_6, tmp_args_element_name_7};
            tmp_call_result_2 = CALL_FUNCTION_WITH_ARGS2(tmp_called_name_4, call_args);
        }

        Py_DECREF(tmp_called_name_4);
        Py_DECREF(tmp_args_element_name_6);
        if (tmp_call_result_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 18;

            goto frame_exception_exit_1;
        }
        Py_DECREF(tmp_call_result_2);
    }

    // Restore frame exception if necessary.
#if 1
    RESTORE_FRAME_EXCEPTION(frame_96ead5d6cba66130403b84e6e9b97e57);
#endif
    popFrameStack();

    assertFrameObject(frame_96ead5d6cba66130403b84e6e9b97e57);

    goto frame_no_exception_1;

    frame_exception_exit_1:;
#if 1
    RESTORE_FRAME_EXCEPTION(frame_96ead5d6cba66130403b84e6e9b97e57);
#endif

    if ( exception_tb == NULL )
    {
        exception_tb = MAKE_TRACEBACK(frame_96ead5d6cba66130403b84e6e9b97e57, exception_lineno);
    }
    else if (exception_tb->tb_frame != &frame_96ead5d6cba66130403b84e6e9b97e57->m_frame) {
        exception_tb = ADD_TRACEBACK(exception_tb, frame_96ead5d6cba66130403b84e6e9b97e57, exception_lineno);
    }

    // Put the previous frame back on top.
    popFrameStack();

    // Return the error.
    goto module_exception_exit;

    frame_no_exception_1:;

    return module_multiprocessing$$45$postLoad;
    module_exception_exit:
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);
    return NULL;
}
