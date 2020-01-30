/* Generated code for Python module 'gio'
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

/* The "_module_gio" is a Python object pointer of module type.
 *
 * Note: For full compatibility with CPython, every module variable access
 * needs to go through it except for cases where the module cannot possibly
 * have changed in the mean time.
 */

PyObject *module_gio;
PyDictObject *moduledict_gio;

/* The declarations of module constants used, if any. */
static PyObject *const_str_plain_NUITKA_PACKAGE_gio;
extern PyObject *const_str_plain_ImportError;
extern PyObject *const_str_plain_get;
static PyObject *const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple;
extern PyObject *const_str_chr_42;
static PyObject *const_str_plain__file_init;
extern PyObject *const_tuple_str_chr_42_tuple;
static PyObject *const_str_plain__app_info_init;
static PyObject *const_str_plain__gio;
static PyObject *const_str_plain_gio;
extern PyObject *const_str_plain_path;
static PyObject *const_tuple_str_plain_GObjectMeta_tuple;
extern PyObject *const_str_digest_5bfaf90dbd407b4fc29090c8f6415242;
static PyObject *const_str_plain_GAppInfoMeta;
extern PyObject *const_str_plain_None;
extern PyObject *const_str_plain___doc__;
extern PyObject *const_int_0;
extern PyObject *const_tuple_empty;
static PyObject *const_str_plain__install_app_info_meta;
extern PyObject *const_str_plain___file__;
static PyObject *const_tuple_0504d048d24c56c3af62d9db60caca5f_tuple;
static PyObject *const_str_digest_b833b3d7c0469253e425e52d59fcd482;
extern PyObject *const_str_plain___module__;
extern PyObject *const_str_plain___path__;
extern PyObject *const_str_plain___call__;
static PyObject *const_str_plain_GFileMeta;
extern PyObject *const_str_plain_gobject;
extern PyObject *const_str_plain___metaclass__;
extern PyObject *const_str_plain_dirname;
static PyObject *const_str_plain_unix;
static PyObject *const_str_digest_5cb4bbad2d2bbc6f0cafe1cf87859014;
extern PyObject *const_str_plain_environ;
static PyObject *const_str_plain__install_file_meta;
extern PyObject *const_str_plain_GObjectMeta;
static PyObject *module_filename_obj;

/* Indicator if this modules private constants were created yet. */
static bool constants_created = false;

/* Function to create module private constants. */
static void createModuleConstants(void) {
    const_str_plain_NUITKA_PACKAGE_gio = UNSTREAM_STRING(&constant_bin[ 26057 ], 18, 1);
    const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple = PyTuple_New(4);
    const_str_plain__app_info_init = UNSTREAM_STRING(&constant_bin[ 1797 ], 14, 1);
    PyTuple_SET_ITEM(const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple, 0, const_str_plain__app_info_init); Py_INCREF(const_str_plain__app_info_init);
    const_str_plain__install_app_info_meta = UNSTREAM_STRING(&constant_bin[ 1833 ], 22, 1);
    PyTuple_SET_ITEM(const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple, 1, const_str_plain__install_app_info_meta); Py_INCREF(const_str_plain__install_app_info_meta);
    const_str_plain__file_init = UNSTREAM_STRING(&constant_bin[ 1694 ], 10, 1);
    PyTuple_SET_ITEM(const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple, 2, const_str_plain__file_init); Py_INCREF(const_str_plain__file_init);
    const_str_plain__install_file_meta = UNSTREAM_STRING(&constant_bin[ 1726 ], 18, 1);
    PyTuple_SET_ITEM(const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple, 3, const_str_plain__install_file_meta); Py_INCREF(const_str_plain__install_file_meta);
    const_str_plain__gio = UNSTREAM_STRING(&constant_bin[ 1635 ], 4, 1);
    const_str_plain_gio = UNSTREAM_STRING(&constant_bin[ 1636 ], 3, 1);
    const_tuple_str_plain_GObjectMeta_tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(const_tuple_str_plain_GObjectMeta_tuple, 0, const_str_plain_GObjectMeta); Py_INCREF(const_str_plain_GObjectMeta);
    const_str_plain_GAppInfoMeta = UNSTREAM_STRING(&constant_bin[ 1877 ], 12, 1);
    const_tuple_0504d048d24c56c3af62d9db60caca5f_tuple = PyTuple_New(2);
    PyTuple_SET_ITEM(const_tuple_0504d048d24c56c3af62d9db60caca5f_tuple, 0, const_str_plain_NUITKA_PACKAGE_gio); Py_INCREF(const_str_plain_NUITKA_PACKAGE_gio);
    PyTuple_SET_ITEM(const_tuple_0504d048d24c56c3af62d9db60caca5f_tuple, 1, const_str_digest_5bfaf90dbd407b4fc29090c8f6415242); Py_INCREF(const_str_digest_5bfaf90dbd407b4fc29090c8f6415242);
    const_str_digest_b833b3d7c0469253e425e52d59fcd482 = UNSTREAM_STRING(&constant_bin[ 26075 ], 12, 0);
    const_str_plain_GFileMeta = UNSTREAM_STRING(&constant_bin[ 1766 ], 9, 1);
    const_str_plain_unix = UNSTREAM_STRING(&constant_bin[ 26087 ], 4, 1);
    const_str_digest_5cb4bbad2d2bbc6f0cafe1cf87859014 = UNSTREAM_STRING(&constant_bin[ 26091 ], 15, 0);

    constants_created = true;
}

/* Function to verify module private constants for non-corruption. */
#ifndef __NUITKA_NO_ASSERT__
void checkModuleConstants_gio(void) {
    // The module may not have been used at all, then ignore this.
    if (constants_created == false) return;


}
#endif

// The module code objects.
static PyCodeObject *codeobj_0eaf5c8946efac81ec709789545b817d;
static PyCodeObject *codeobj_372f6a52d8e1ab34c1a1fea199f8c1d3;
static PyCodeObject *codeobj_076f849ef1289a16552ae03e70534406;

static void createModuleCodeObjects(void) {
    module_filename_obj = MAKE_RELATIVE_PATH(const_str_digest_5cb4bbad2d2bbc6f0cafe1cf87859014);
    codeobj_0eaf5c8946efac81ec709789545b817d = MAKE_CODEOBJECT(module_filename_obj, 1, CO_NOFREE, const_str_digest_b833b3d7c0469253e425e52d59fcd482, const_tuple_empty, 0, 0, 0);
    codeobj_372f6a52d8e1ab34c1a1fea199f8c1d3 = MAKE_CODEOBJECT(module_filename_obj, 38, CO_NEWLOCALS | CO_NOFREE, const_str_plain_GAppInfoMeta, const_tuple_empty, 0, 0, 0);
    codeobj_076f849ef1289a16552ae03e70534406 = MAKE_CODEOBJECT(module_filename_obj, 34, CO_NEWLOCALS | CO_NOFREE, const_str_plain_GFileMeta, const_tuple_empty, 0, 0, 0);
}

// The module function declarations.


// The module function definitions.



#if PYTHON_VERSION >= 300
static struct PyModuleDef mdef_gio =
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

function_impl_code functable_gio[] = {

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

    function_impl_code *current = functable_gio;
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

    if (offset > sizeof(functable_gio) || offset < 0) {
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
        functable_gio[offset],
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
        module_gio,
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
PyObject *modulecode_gio(char const *module_full_name) {
#if defined(_NUITKA_EXE) || PYTHON_VERSION >= 300
    static bool _init_done = false;

    // Modules might be imported repeatedly, which is to be ignored.
    if (_init_done) {
        return module_gio;
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
    PRINT_STRING("gio: Calling setupMetaPathBasedLoader().\n");
#endif
    setupMetaPathBasedLoader();

#if PYTHON_VERSION >= 300
    patchInspectModule();
#endif

#endif

    /* The constants only used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("gio: Calling createModuleConstants().\n");
#endif
    createModuleConstants();

    /* The code objects used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("gio: Calling createModuleCodeObjects().\n");
#endif
    createModuleCodeObjects();

    // PRINT_STRING("in initgio\n");

    // Create the module object first. There are no methods initially, all are
    // added dynamically in actual code only.  Also no "__doc__" is initially
    // set at this time, as it could not contain NUL characters this way, they
    // are instead set in early module code.  No "self" for modules, we have no
    // use for it.
#if PYTHON_VERSION < 300
    module_gio = Py_InitModule4(
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
    mdef_gio.m_name = module_full_name;
    module_gio = PyModule_Create(&mdef_gio);
#endif

    moduledict_gio = MODULE_DICT(module_gio);

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
        moduledict_gio,
        (Nuitka_StringObject *)const_str_plain___compiled__,
        Nuitka_dunder_compiled_value
    );

    // Update "__package__" value to what it ought to be.
    {
#if 0
        UPDATE_STRING_DICT0(
            moduledict_gio,
            (Nuitka_StringObject *)const_str_plain___package__,
            const_str_empty
        );
#elif 1
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain___name__);

        UPDATE_STRING_DICT0(
            moduledict_gio,
            (Nuitka_StringObject *)const_str_plain___package__,
            module_name
        );
#else

#if PYTHON_VERSION < 300
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain___name__);
        char const *module_name_cstr = PyString_AS_STRING(module_name);

        char const *last_dot = strrchr(module_name_cstr, '.');

        if (last_dot != NULL)
        {
            UPDATE_STRING_DICT1(
                moduledict_gio,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyString_FromStringAndSize(module_name_cstr, last_dot - module_name_cstr)
            );
        }
#else
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain___name__);
        Py_ssize_t dot_index = PyUnicode_Find(module_name, const_str_dot, 0, PyUnicode_GetLength(module_name), -1);

        if (dot_index != -1)
        {
            UPDATE_STRING_DICT1(
                moduledict_gio,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyUnicode_Substring(module_name, 0, dot_index)
            );
        }
#endif
#endif
    }

    CHECK_OBJECT(module_gio);

// Seems to work for Python2.7 out of the box, but for Python3, the module
// doesn't automatically enter "sys.modules", so do it manually.
#if PYTHON_VERSION >= 300
    {
        int r = PyDict_SetItemString(PyImport_GetModuleDict(), module_full_name, module_gio);
        assert(r != -1);
    }
#endif

    // For deep importing of a module we need to have "__builtins__", so we set
    // it ourselves in the same way than CPython does. Note: This must be done
    // before the frame object is allocated, or else it may fail.

    if (GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain___builtins__) == NULL)
    {
        PyObject *value = (PyObject *)builtin_module;

        // Check if main module, not a dict then but the module itself.
#if !defined(_NUITKA_EXE) || !0
        value = PyModule_GetDict(value);
#endif

        UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain___builtins__, value);
    }

#if PYTHON_VERSION >= 300
    UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain___loader__, (PyObject *)&Nuitka_Loader_Type);
#endif

#if PYTHON_VERSION >= 340
// Set the "__spec__" value

#if 0
    // Main modules just get "None" as spec.
    UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain___spec__, Py_None);
#else
    // Other modules get a "ModuleSpec" from the standard mechanism.
    {
        PyObject *bootstrap_module = PyImport_ImportModule("importlib._bootstrap");
        CHECK_OBJECT(bootstrap_module);
        PyObject *module_spec_class = PyObject_GetAttrString(bootstrap_module, "ModuleSpec");
        Py_DECREF(bootstrap_module);

        PyObject *args[] = {
            GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain___name__),
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
#if 1
        SET_ATTRIBUTE(spec_value, const_str_plain_submodule_search_locations, PyList_New(0));
#endif

// Mark the execution in the "__spec__" value.
        SET_ATTRIBUTE(spec_value, const_str_plain__initializing, Py_True);

        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain___spec__, spec_value);
    }
#endif
#endif

    // Temp variables if any
    PyObject *tmp_class_creation_1__bases = NULL;
    PyObject *tmp_class_creation_1__class = NULL;
    PyObject *tmp_class_creation_1__class_dict = NULL;
    PyObject *tmp_class_creation_1__metaclass = NULL;
    PyObject *tmp_class_creation_2__bases = NULL;
    PyObject *tmp_class_creation_2__class = NULL;
    PyObject *tmp_class_creation_2__class_dict = NULL;
    PyObject *tmp_class_creation_2__metaclass = NULL;
    PyObject *tmp_import_from_1__module = NULL;
    PyObject *tmp_select_metaclass_1__base = NULL;
    PyObject *tmp_select_metaclass_2__base = NULL;
    struct Nuitka_FrameObject *frame_0eaf5c8946efac81ec709789545b817d;
    NUITKA_MAY_BE_UNUSED char const *type_description_1 = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    bool tmp_result;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
    int tmp_res;
    PyObject *locals_gio_34 = NULL;
    PyObject *tmp_dictset_value;
    struct Nuitka_FrameObject *frame_076f849ef1289a16552ae03e70534406_2;
    NUITKA_MAY_BE_UNUSED char const *type_description_2 = NULL;
    static struct Nuitka_FrameObject *cache_frame_076f849ef1289a16552ae03e70534406_2 = NULL;
    PyObject *exception_keeper_type_3;
    PyObject *exception_keeper_value_3;
    PyTracebackObject *exception_keeper_tb_3;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_3;
    PyObject *exception_keeper_type_4;
    PyObject *exception_keeper_value_4;
    PyTracebackObject *exception_keeper_tb_4;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_4;
    PyObject *exception_keeper_type_5;
    PyObject *exception_keeper_value_5;
    PyTracebackObject *exception_keeper_tb_5;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_5;
    PyObject *locals_gio_38 = NULL;
    struct Nuitka_FrameObject *frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3;
    NUITKA_MAY_BE_UNUSED char const *type_description_3 = NULL;
    static struct Nuitka_FrameObject *cache_frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3 = NULL;
    PyObject *exception_keeper_type_6;
    PyObject *exception_keeper_value_6;
    PyTracebackObject *exception_keeper_tb_6;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_6;
    PyObject *exception_keeper_type_7;
    PyObject *exception_keeper_value_7;
    PyTracebackObject *exception_keeper_tb_7;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_7;
    PyObject *exception_keeper_type_8;
    PyObject *exception_keeper_value_8;
    PyTracebackObject *exception_keeper_tb_8;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_8;

    // Module code.
    {
        PyObject *tmp_assign_source_1;
        tmp_assign_source_1 = Py_None;
        UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain___doc__, tmp_assign_source_1);
    }
    {
        PyObject *tmp_assign_source_2;
        tmp_assign_source_2 = module_filename_obj;
        UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain___file__, tmp_assign_source_2);
    }
    // Frame without reuse.
    frame_0eaf5c8946efac81ec709789545b817d = MAKE_MODULE_FRAME(codeobj_0eaf5c8946efac81ec709789545b817d, module_gio);

    // Push the new frame as the currently active one, and we should be exclusively
    // owning it.
    pushFrameStack(frame_0eaf5c8946efac81ec709789545b817d);
    assert(Py_REFCNT(frame_0eaf5c8946efac81ec709789545b817d) == 2);

    // Framed code:
    {
        PyObject *tmp_assign_source_3;
        PyObject *tmp_list_element_1;
        PyObject *tmp_called_name_1;
        PyObject *tmp_source_name_1;
        PyObject *tmp_args_element_name_1;
        PyObject *tmp_called_instance_1;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 1;
        {
            PyObject *module = PyImport_ImportModule("os");
            if (likely(module != NULL)) {
                tmp_source_name_1 = PyObject_GetAttr(module, const_str_plain_path);
            } else {
                tmp_source_name_1 = NULL;
            }
        }

        if (tmp_source_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        tmp_called_name_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain_dirname);
        if (tmp_called_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        tmp_args_element_name_1 = module_filename_obj;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 1;
        tmp_list_element_1 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_1, tmp_args_element_name_1);
        Py_DECREF(tmp_called_name_1);
        if (tmp_list_element_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        tmp_assign_source_3 = PyList_New(2);
        PyList_SET_ITEM(tmp_assign_source_3, 0, tmp_list_element_1);
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 1;
        {
            PyObject *module = PyImport_ImportModule("os");
            if (likely(module != NULL)) {
                tmp_called_instance_1 = PyObject_GetAttr(module, const_str_plain_environ);
            } else {
                tmp_called_instance_1 = NULL;
            }
        }

        if (tmp_called_instance_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_assign_source_3);

            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 1;
        tmp_list_element_1 = CALL_METHOD_WITH_ARGS2(tmp_called_instance_1, const_str_plain_get, &PyTuple_GET_ITEM(const_tuple_0504d048d24c56c3af62d9db60caca5f_tuple, 0));

        if (tmp_list_element_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_assign_source_3);

            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        PyList_SET_ITEM(tmp_assign_source_3, 1, tmp_list_element_1);
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain___path__, tmp_assign_source_3);
    }
    {
        PyObject *tmp_assign_source_4;
        PyObject *tmp_import_name_from_1;
        PyObject *tmp_name_name_1;
        PyObject *tmp_globals_name_1;
        PyObject *tmp_locals_name_1;
        PyObject *tmp_fromlist_name_1;
        tmp_name_name_1 = const_str_plain_gobject;
        tmp_globals_name_1 = (PyObject *)moduledict_gio;
        tmp_locals_name_1 = Py_None;
        tmp_fromlist_name_1 = const_tuple_str_plain_GObjectMeta_tuple;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 22;
        tmp_import_name_from_1 = IMPORT_MODULE4(tmp_name_name_1, tmp_globals_name_1, tmp_locals_name_1, tmp_fromlist_name_1);
        if (tmp_import_name_from_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 22;

            goto frame_exception_exit_1;
        }
        tmp_assign_source_4 = IMPORT_NAME(tmp_import_name_from_1, const_str_plain_GObjectMeta);
        Py_DECREF(tmp_import_name_from_1);
        if (tmp_assign_source_4 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 22;

            goto frame_exception_exit_1;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GObjectMeta, tmp_assign_source_4);
    }
    {
        PyObject *tmp_star_imported_1;
        PyObject *tmp_name_name_2;
        PyObject *tmp_globals_name_2;
        PyObject *tmp_locals_name_2;
        PyObject *tmp_fromlist_name_2;
        tmp_name_name_2 = const_str_plain__gio;
        tmp_globals_name_2 = (PyObject *)moduledict_gio;
        tmp_locals_name_2 = (PyObject *)moduledict_gio;
        tmp_fromlist_name_2 = const_tuple_str_chr_42_tuple;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 23;
        tmp_star_imported_1 = IMPORT_MODULE4(tmp_name_name_2, tmp_globals_name_2, tmp_locals_name_2, tmp_fromlist_name_2);
        if (tmp_star_imported_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 23;

            goto frame_exception_exit_1;
        }
        tmp_result = IMPORT_MODULE_STAR(module_gio, true, tmp_star_imported_1);
        Py_DECREF(tmp_star_imported_1);
        if (tmp_result == false) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 23;

            goto frame_exception_exit_1;
        }
    }
    {
        PyObject *tmp_assign_source_5;
        PyObject *tmp_name_name_3;
        PyObject *tmp_globals_name_3;
        PyObject *tmp_locals_name_3;
        PyObject *tmp_fromlist_name_3;
        tmp_name_name_3 = const_str_plain__gio;
        tmp_globals_name_3 = (PyObject *)moduledict_gio;
        tmp_locals_name_3 = Py_None;
        tmp_fromlist_name_3 = const_tuple_14186f5ffa6bafccd706e0b597e5548e_tuple;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 24;
        tmp_assign_source_5 = IMPORT_MODULE4(tmp_name_name_3, tmp_globals_name_3, tmp_locals_name_3, tmp_fromlist_name_3);
        if (tmp_assign_source_5 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 24;

            goto frame_exception_exit_1;
        }
        assert(tmp_import_from_1__module == NULL);
        tmp_import_from_1__module = tmp_assign_source_5;
    }
    // Tried code:
    {
        PyObject *tmp_assign_source_6;
        PyObject *tmp_import_name_from_2;
        CHECK_OBJECT(tmp_import_from_1__module);
        tmp_import_name_from_2 = tmp_import_from_1__module;
        tmp_assign_source_6 = IMPORT_NAME(tmp_import_name_from_2, const_str_plain__app_info_init);
        if (tmp_assign_source_6 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 24;

            goto try_except_handler_1;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain__app_info_init, tmp_assign_source_6);
    }
    {
        PyObject *tmp_assign_source_7;
        PyObject *tmp_import_name_from_3;
        CHECK_OBJECT(tmp_import_from_1__module);
        tmp_import_name_from_3 = tmp_import_from_1__module;
        tmp_assign_source_7 = IMPORT_NAME(tmp_import_name_from_3, const_str_plain__install_app_info_meta);
        if (tmp_assign_source_7 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 24;

            goto try_except_handler_1;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain__install_app_info_meta, tmp_assign_source_7);
    }
    {
        PyObject *tmp_assign_source_8;
        PyObject *tmp_import_name_from_4;
        CHECK_OBJECT(tmp_import_from_1__module);
        tmp_import_name_from_4 = tmp_import_from_1__module;
        tmp_assign_source_8 = IMPORT_NAME(tmp_import_name_from_4, const_str_plain__file_init);
        if (tmp_assign_source_8 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 24;

            goto try_except_handler_1;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain__file_init, tmp_assign_source_8);
    }
    {
        PyObject *tmp_assign_source_9;
        PyObject *tmp_import_name_from_5;
        CHECK_OBJECT(tmp_import_from_1__module);
        tmp_import_name_from_5 = tmp_import_from_1__module;
        tmp_assign_source_9 = IMPORT_NAME(tmp_import_name_from_5, const_str_plain__install_file_meta);
        if (tmp_assign_source_9 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 24;

            goto try_except_handler_1;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain__install_file_meta, tmp_assign_source_9);
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

    CHECK_OBJECT((PyObject *)tmp_import_from_1__module);
    Py_DECREF(tmp_import_from_1__module);
    tmp_import_from_1__module = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_1;
    exception_value = exception_keeper_value_1;
    exception_tb = exception_keeper_tb_1;
    exception_lineno = exception_keeper_lineno_1;

    goto frame_exception_exit_1;
    // End of try:
    try_end_1:;
    CHECK_OBJECT((PyObject *)tmp_import_from_1__module);
    Py_DECREF(tmp_import_from_1__module);
    tmp_import_from_1__module = NULL;

    // Tried code:
    {
        PyObject *tmp_assign_source_10;
        PyObject *tmp_name_name_4;
        PyObject *tmp_globals_name_4;
        PyObject *tmp_locals_name_4;
        PyObject *tmp_fromlist_name_4;
        tmp_name_name_4 = const_str_plain_unix;
        tmp_globals_name_4 = (PyObject *)moduledict_gio;
        tmp_locals_name_4 = Py_None;
        tmp_fromlist_name_4 = Py_None;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 28;
        tmp_assign_source_10 = IMPORT_MODULE4(tmp_name_name_4, tmp_globals_name_4, tmp_locals_name_4, tmp_fromlist_name_4);
        if (tmp_assign_source_10 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 28;

            goto try_except_handler_2;
        }
        UPDATE_STRING_DICT1(moduledict_gio, (Nuitka_StringObject *)const_str_plain_unix, tmp_assign_source_10);
    }
    goto try_end_2;
    // Exception handler code:
    try_except_handler_2:;
    exception_keeper_type_2 = exception_type;
    exception_keeper_value_2 = exception_value;
    exception_keeper_tb_2 = exception_tb;
    exception_keeper_lineno_2 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    // Preserve existing published exception.
    PRESERVE_FRAME_EXCEPTION(frame_0eaf5c8946efac81ec709789545b817d);
    if (exception_keeper_tb_2 == NULL) {
        exception_keeper_tb_2 = MAKE_TRACEBACK(frame_0eaf5c8946efac81ec709789545b817d, exception_keeper_lineno_2);
    } else if (exception_keeper_lineno_2 != 0) {
        exception_keeper_tb_2 = ADD_TRACEBACK(exception_keeper_tb_2, frame_0eaf5c8946efac81ec709789545b817d, exception_keeper_lineno_2);
    }

    NORMALIZE_EXCEPTION(&exception_keeper_type_2, &exception_keeper_value_2, &exception_keeper_tb_2);
    PUBLISH_EXCEPTION(&exception_keeper_type_2, &exception_keeper_value_2, &exception_keeper_tb_2);
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_mvar_value_1;
        tmp_compexpr_left_1 = EXC_TYPE(PyThreadState_GET());
        tmp_mvar_value_1 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_ImportError);

        if (unlikely(tmp_mvar_value_1 == NULL)) {
            tmp_mvar_value_1 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_ImportError);
        }

        if (tmp_mvar_value_1 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1570 ], 33, 0);
            exception_tb = NULL;

            exception_lineno = 30;

            goto frame_exception_exit_1;
        }

        tmp_compexpr_right_1 = tmp_mvar_value_1;
        tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_1, tmp_compexpr_right_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 30;

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
            PyObject *tmp_assign_source_11;
            PyObject *tmp_mvar_value_2;
            tmp_mvar_value_2 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_None);

            if (unlikely(tmp_mvar_value_2 == NULL)) {
                tmp_mvar_value_2 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_None);
            }

            if (tmp_mvar_value_2 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 1603 ], 26, 0);
                exception_tb = NULL;

                exception_lineno = 31;

                goto frame_exception_exit_1;
            }

            tmp_assign_source_11 = tmp_mvar_value_2;
            UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain_unix, tmp_assign_source_11);
        }
        goto branch_end_1;
        branch_no_1:;
        tmp_result = RERAISE_EXCEPTION(&exception_type, &exception_value, &exception_tb);
        if (unlikely(tmp_result == false)) {
            exception_lineno = 27;
        }

        if (exception_tb && exception_tb->tb_frame == &frame_0eaf5c8946efac81ec709789545b817d->m_frame) frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = exception_tb->tb_lineno;

        goto frame_exception_exit_1;
        branch_end_1:;
    }
    goto try_end_2;
    // exception handler codes exits in all cases
    NUITKA_CANNOT_GET_HERE(gio);
    return NULL;
    // End of try:
    try_end_2:;
    tmp_res = PyDict_DelItem((PyObject *)moduledict_gio, const_str_plain__gio);
    tmp_result = tmp_res != -1;
    if ( tmp_result == false ) CLEAR_ERROR_OCCURRED();

    if (tmp_result == false) {

        exception_type = PyExc_NameError;
        Py_INCREF(exception_type);
        exception_value = UNSTREAM_STRING(&constant_bin[ 1629 ], 26, 0);
        exception_tb = NULL;

        exception_lineno = 32;

        goto frame_exception_exit_1;
    }

    // Tried code:
    {
        PyObject *tmp_assign_source_12;
        PyObject *tmp_tuple_element_1;
        PyObject *tmp_mvar_value_3;
        tmp_mvar_value_3 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GObjectMeta);

        if (unlikely(tmp_mvar_value_3 == NULL)) {
            tmp_mvar_value_3 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_GObjectMeta);
        }

        if (tmp_mvar_value_3 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1655 ], 33, 0);
            exception_tb = NULL;

            exception_lineno = 34;

            goto try_except_handler_3;
        }

        tmp_tuple_element_1 = tmp_mvar_value_3;
        tmp_assign_source_12 = PyTuple_New(1);
        Py_INCREF(tmp_tuple_element_1);
        PyTuple_SET_ITEM(tmp_assign_source_12, 0, tmp_tuple_element_1);
        assert(tmp_class_creation_1__bases == NULL);
        tmp_class_creation_1__bases = tmp_assign_source_12;
    }
    {
        PyObject *tmp_assign_source_13;
        {
            PyObject *tmp_set_locals_1;
            tmp_set_locals_1 = PyDict_New();
            locals_gio_34 = tmp_set_locals_1;
        }
        tmp_dictset_value = const_str_plain_gio;
        tmp_res = PyDict_SetItem(locals_gio_34, const_str_plain___module__, tmp_dictset_value);
        assert(!(tmp_res != 0));
        // Tried code:
        MAKE_OR_REUSE_FRAME(cache_frame_076f849ef1289a16552ae03e70534406_2, codeobj_076f849ef1289a16552ae03e70534406, module_gio, 0);
        frame_076f849ef1289a16552ae03e70534406_2 = cache_frame_076f849ef1289a16552ae03e70534406_2;

        // Push the new frame as the currently active one.
        pushFrameStack(frame_076f849ef1289a16552ae03e70534406_2);

        // Mark the frame object as in use, ref count 1 will be up for reuse.
        assert(Py_REFCNT(frame_076f849ef1289a16552ae03e70534406_2) == 2); // Frame stack

        // Framed code:
        {
            PyObject *tmp_mvar_value_4;
            tmp_mvar_value_4 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain__file_init);

            if (unlikely(tmp_mvar_value_4 == NULL)) {
                tmp_mvar_value_4 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__file_init);
            }

            if (tmp_mvar_value_4 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 1688 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 35;

                goto frame_exception_exit_2;
            }

            tmp_dictset_value = tmp_mvar_value_4;
            tmp_res = PyDict_SetItem(locals_gio_34, const_str_plain___call__, tmp_dictset_value);
            if (tmp_res != 0) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 35;

                goto frame_exception_exit_2;
            }
        }

#if 0
        RESTORE_FRAME_EXCEPTION(frame_076f849ef1289a16552ae03e70534406_2);
#endif

        // Put the previous frame back on top.
        popFrameStack();

        goto frame_no_exception_1;

        frame_exception_exit_2:;

#if 0
        RESTORE_FRAME_EXCEPTION(frame_076f849ef1289a16552ae03e70534406_2);
#endif

        if (exception_tb == NULL) {
            exception_tb = MAKE_TRACEBACK(frame_076f849ef1289a16552ae03e70534406_2, exception_lineno);
        }
        else if (exception_tb->tb_frame != &frame_076f849ef1289a16552ae03e70534406_2->m_frame) {
            exception_tb = ADD_TRACEBACK(exception_tb, frame_076f849ef1289a16552ae03e70534406_2, exception_lineno);
        }

        // Attachs locals to frame if any.
        Nuitka_Frame_AttachLocals(
            (struct Nuitka_FrameObject *)frame_076f849ef1289a16552ae03e70534406_2,
            type_description_2
        );


        // Release cached frame.
        if (frame_076f849ef1289a16552ae03e70534406_2 == cache_frame_076f849ef1289a16552ae03e70534406_2) {
            Py_DECREF(frame_076f849ef1289a16552ae03e70534406_2);
        }
        cache_frame_076f849ef1289a16552ae03e70534406_2 = NULL;

        assertFrameObject(frame_076f849ef1289a16552ae03e70534406_2);

        // Put the previous frame back on top.
        popFrameStack();

        // Return the error.
        goto nested_frame_exit_1;

        frame_no_exception_1:;
        goto skip_nested_handling_1;
        nested_frame_exit_1:;

        goto try_except_handler_4;
        skip_nested_handling_1:;
        tmp_assign_source_13 = locals_gio_34;
        Py_INCREF(tmp_assign_source_13);
        goto try_return_handler_4;
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Return handler code:
        try_return_handler_4:;
        Py_DECREF(locals_gio_34);
        locals_gio_34 = NULL;
        goto outline_result_1;
        // Exception handler code:
        try_except_handler_4:;
        exception_keeper_type_3 = exception_type;
        exception_keeper_value_3 = exception_value;
        exception_keeper_tb_3 = exception_tb;
        exception_keeper_lineno_3 = exception_lineno;
        exception_type = NULL;
        exception_value = NULL;
        exception_tb = NULL;
        exception_lineno = 0;

        Py_DECREF(locals_gio_34);
        locals_gio_34 = NULL;
        // Re-raise.
        exception_type = exception_keeper_type_3;
        exception_value = exception_keeper_value_3;
        exception_tb = exception_keeper_tb_3;
        exception_lineno = exception_keeper_lineno_3;

        goto outline_exception_1;
        // End of try:
        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        outline_exception_1:;
        exception_lineno = 34;
        goto try_except_handler_3;
        outline_result_1:;
        assert(tmp_class_creation_1__class_dict == NULL);
        tmp_class_creation_1__class_dict = tmp_assign_source_13;
    }
    {
        PyObject *tmp_assign_source_14;
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


            exception_lineno = 34;

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
        tmp_assign_source_14 = DICT_GET_ITEM(tmp_dict_name_2, tmp_key_name_2);
        if (tmp_assign_source_14 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 34;

            goto try_except_handler_3;
        }
        goto condexpr_end_1;
        condexpr_false_1:;
        {
            PyObject *tmp_assign_source_15;
            PyObject *tmp_subscribed_name_1;
            PyObject *tmp_subscript_name_1;
            CHECK_OBJECT(tmp_class_creation_1__bases);
            tmp_subscribed_name_1 = tmp_class_creation_1__bases;
            tmp_subscript_name_1 = const_int_0;
            tmp_assign_source_15 = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_1, tmp_subscript_name_1, 0);
            if (tmp_assign_source_15 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 34;

                goto try_except_handler_3;
            }
            assert(tmp_select_metaclass_1__base == NULL);
            tmp_select_metaclass_1__base = tmp_assign_source_15;
        }
        // Tried code:
        // Tried code:
        {
            PyObject *tmp_source_name_2;
            CHECK_OBJECT(tmp_select_metaclass_1__base);
            tmp_source_name_2 = tmp_select_metaclass_1__base;
            tmp_assign_source_14 = LOOKUP_ATTRIBUTE_CLASS_SLOT(tmp_source_name_2);
            if (tmp_assign_source_14 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 34;

                goto try_except_handler_6;
            }
            goto try_return_handler_5;
        }
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Exception handler code:
        try_except_handler_6:;
        exception_keeper_type_4 = exception_type;
        exception_keeper_value_4 = exception_value;
        exception_keeper_tb_4 = exception_tb;
        exception_keeper_lineno_4 = exception_lineno;
        exception_type = NULL;
        exception_value = NULL;
        exception_tb = NULL;
        exception_lineno = 0;

        {
            PyObject *tmp_type_arg_1;
            Py_DECREF(exception_keeper_type_4);
            Py_XDECREF(exception_keeper_value_4);
            Py_XDECREF(exception_keeper_tb_4);
            CHECK_OBJECT(tmp_select_metaclass_1__base);
            tmp_type_arg_1 = tmp_select_metaclass_1__base;
            tmp_assign_source_14 = BUILTIN_TYPE1(tmp_type_arg_1);
            assert(!(tmp_assign_source_14 == NULL));
            goto try_return_handler_5;
        }
        // End of try:
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Return handler code:
        try_return_handler_5:;
        CHECK_OBJECT((PyObject *)tmp_select_metaclass_1__base);
        Py_DECREF(tmp_select_metaclass_1__base);
        tmp_select_metaclass_1__base = NULL;

        goto outline_result_2;
        // End of try:
        CHECK_OBJECT((PyObject *)tmp_select_metaclass_1__base);
        Py_DECREF(tmp_select_metaclass_1__base);
        tmp_select_metaclass_1__base = NULL;

        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        outline_result_2:;
        condexpr_end_1:;
        assert(tmp_class_creation_1__metaclass == NULL);
        tmp_class_creation_1__metaclass = tmp_assign_source_14;
    }
    {
        PyObject *tmp_assign_source_16;
        PyObject *tmp_called_name_2;
        PyObject *tmp_args_element_name_2;
        PyObject *tmp_args_element_name_3;
        PyObject *tmp_args_element_name_4;
        CHECK_OBJECT(tmp_class_creation_1__metaclass);
        tmp_called_name_2 = tmp_class_creation_1__metaclass;
        tmp_args_element_name_2 = const_str_plain_GFileMeta;
        CHECK_OBJECT(tmp_class_creation_1__bases);
        tmp_args_element_name_3 = tmp_class_creation_1__bases;
        CHECK_OBJECT(tmp_class_creation_1__class_dict);
        tmp_args_element_name_4 = tmp_class_creation_1__class_dict;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 34;
        {
            PyObject *call_args[] = {tmp_args_element_name_2, tmp_args_element_name_3, tmp_args_element_name_4};
            tmp_assign_source_16 = CALL_FUNCTION_WITH_ARGS3(tmp_called_name_2, call_args);
        }

        if (tmp_assign_source_16 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 34;

            goto try_except_handler_3;
        }
        assert(tmp_class_creation_1__class == NULL);
        tmp_class_creation_1__class = tmp_assign_source_16;
    }
    goto try_end_3;
    // Exception handler code:
    try_except_handler_3:;
    exception_keeper_type_5 = exception_type;
    exception_keeper_value_5 = exception_value;
    exception_keeper_tb_5 = exception_tb;
    exception_keeper_lineno_5 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(tmp_class_creation_1__bases);
    tmp_class_creation_1__bases = NULL;

    Py_XDECREF(tmp_class_creation_1__class_dict);
    tmp_class_creation_1__class_dict = NULL;

    Py_XDECREF(tmp_class_creation_1__metaclass);
    tmp_class_creation_1__metaclass = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_5;
    exception_value = exception_keeper_value_5;
    exception_tb = exception_keeper_tb_5;
    exception_lineno = exception_keeper_lineno_5;

    goto frame_exception_exit_1;
    // End of try:
    try_end_3:;
    {
        PyObject *tmp_assign_source_17;
        CHECK_OBJECT(tmp_class_creation_1__class);
        tmp_assign_source_17 = tmp_class_creation_1__class;
        UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GFileMeta, tmp_assign_source_17);
    }
    CHECK_OBJECT((PyObject *)tmp_class_creation_1__class);
    Py_DECREF(tmp_class_creation_1__class);
    tmp_class_creation_1__class = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__bases);
    Py_DECREF(tmp_class_creation_1__bases);
    tmp_class_creation_1__bases = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__class_dict);
    Py_DECREF(tmp_class_creation_1__class_dict);
    tmp_class_creation_1__class_dict = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_1__metaclass);
    Py_DECREF(tmp_class_creation_1__metaclass);
    tmp_class_creation_1__metaclass = NULL;

    {
        PyObject *tmp_called_name_3;
        PyObject *tmp_mvar_value_5;
        PyObject *tmp_call_result_1;
        PyObject *tmp_args_element_name_5;
        PyObject *tmp_mvar_value_6;
        tmp_mvar_value_5 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain__install_file_meta);

        if (unlikely(tmp_mvar_value_5 == NULL)) {
            tmp_mvar_value_5 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__install_file_meta);
        }

        if (tmp_mvar_value_5 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1720 ], 40, 0);
            exception_tb = NULL;

            exception_lineno = 36;

            goto frame_exception_exit_1;
        }

        tmp_called_name_3 = tmp_mvar_value_5;
        tmp_mvar_value_6 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GFileMeta);

        if (unlikely(tmp_mvar_value_6 == NULL)) {
            tmp_mvar_value_6 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_GFileMeta);
        }

        if (tmp_mvar_value_6 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1760 ], 31, 0);
            exception_tb = NULL;

            exception_lineno = 36;

            goto frame_exception_exit_1;
        }

        tmp_args_element_name_5 = tmp_mvar_value_6;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 36;
        tmp_call_result_1 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_3, tmp_args_element_name_5);
        if (tmp_call_result_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 36;

            goto frame_exception_exit_1;
        }
        Py_DECREF(tmp_call_result_1);
    }
    // Tried code:
    {
        PyObject *tmp_assign_source_18;
        PyObject *tmp_tuple_element_2;
        PyObject *tmp_mvar_value_7;
        tmp_mvar_value_7 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GObjectMeta);

        if (unlikely(tmp_mvar_value_7 == NULL)) {
            tmp_mvar_value_7 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_GObjectMeta);
        }

        if (tmp_mvar_value_7 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1655 ], 33, 0);
            exception_tb = NULL;

            exception_lineno = 38;

            goto try_except_handler_7;
        }

        tmp_tuple_element_2 = tmp_mvar_value_7;
        tmp_assign_source_18 = PyTuple_New(1);
        Py_INCREF(tmp_tuple_element_2);
        PyTuple_SET_ITEM(tmp_assign_source_18, 0, tmp_tuple_element_2);
        assert(tmp_class_creation_2__bases == NULL);
        tmp_class_creation_2__bases = tmp_assign_source_18;
    }
    {
        PyObject *tmp_assign_source_19;
        {
            PyObject *tmp_set_locals_2;
            tmp_set_locals_2 = PyDict_New();
            locals_gio_38 = tmp_set_locals_2;
        }
        tmp_dictset_value = const_str_plain_gio;
        tmp_res = PyDict_SetItem(locals_gio_38, const_str_plain___module__, tmp_dictset_value);
        assert(!(tmp_res != 0));
        // Tried code:
        MAKE_OR_REUSE_FRAME(cache_frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3, codeobj_372f6a52d8e1ab34c1a1fea199f8c1d3, module_gio, 0);
        frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3 = cache_frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3;

        // Push the new frame as the currently active one.
        pushFrameStack(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3);

        // Mark the frame object as in use, ref count 1 will be up for reuse.
        assert(Py_REFCNT(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3) == 2); // Frame stack

        // Framed code:
        {
            PyObject *tmp_mvar_value_8;
            tmp_mvar_value_8 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain__app_info_init);

            if (unlikely(tmp_mvar_value_8 == NULL)) {
                tmp_mvar_value_8 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__app_info_init);
            }

            if (tmp_mvar_value_8 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 1791 ], 36, 0);
                exception_tb = NULL;

                exception_lineno = 39;

                goto frame_exception_exit_3;
            }

            tmp_dictset_value = tmp_mvar_value_8;
            tmp_res = PyDict_SetItem(locals_gio_38, const_str_plain___call__, tmp_dictset_value);
            if (tmp_res != 0) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 39;

                goto frame_exception_exit_3;
            }
        }

#if 0
        RESTORE_FRAME_EXCEPTION(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3);
#endif

        // Put the previous frame back on top.
        popFrameStack();

        goto frame_no_exception_2;

        frame_exception_exit_3:;

#if 0
        RESTORE_FRAME_EXCEPTION(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3);
#endif

        if (exception_tb == NULL) {
            exception_tb = MAKE_TRACEBACK(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3, exception_lineno);
        }
        else if (exception_tb->tb_frame != &frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3->m_frame) {
            exception_tb = ADD_TRACEBACK(exception_tb, frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3, exception_lineno);
        }

        // Attachs locals to frame if any.
        Nuitka_Frame_AttachLocals(
            (struct Nuitka_FrameObject *)frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3,
            type_description_2
        );


        // Release cached frame.
        if (frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3 == cache_frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3) {
            Py_DECREF(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3);
        }
        cache_frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3 = NULL;

        assertFrameObject(frame_372f6a52d8e1ab34c1a1fea199f8c1d3_3);

        // Put the previous frame back on top.
        popFrameStack();

        // Return the error.
        goto nested_frame_exit_2;

        frame_no_exception_2:;
        goto skip_nested_handling_2;
        nested_frame_exit_2:;

        goto try_except_handler_8;
        skip_nested_handling_2:;
        tmp_assign_source_19 = locals_gio_38;
        Py_INCREF(tmp_assign_source_19);
        goto try_return_handler_8;
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Return handler code:
        try_return_handler_8:;
        Py_DECREF(locals_gio_38);
        locals_gio_38 = NULL;
        goto outline_result_3;
        // Exception handler code:
        try_except_handler_8:;
        exception_keeper_type_6 = exception_type;
        exception_keeper_value_6 = exception_value;
        exception_keeper_tb_6 = exception_tb;
        exception_keeper_lineno_6 = exception_lineno;
        exception_type = NULL;
        exception_value = NULL;
        exception_tb = NULL;
        exception_lineno = 0;

        Py_DECREF(locals_gio_38);
        locals_gio_38 = NULL;
        // Re-raise.
        exception_type = exception_keeper_type_6;
        exception_value = exception_keeper_value_6;
        exception_tb = exception_keeper_tb_6;
        exception_lineno = exception_keeper_lineno_6;

        goto outline_exception_2;
        // End of try:
        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        outline_exception_2:;
        exception_lineno = 38;
        goto try_except_handler_7;
        outline_result_3:;
        assert(tmp_class_creation_2__class_dict == NULL);
        tmp_class_creation_2__class_dict = tmp_assign_source_19;
    }
    {
        PyObject *tmp_assign_source_20;
        nuitka_bool tmp_condition_result_3;
        PyObject *tmp_key_name_3;
        PyObject *tmp_dict_name_3;
        PyObject *tmp_dict_name_4;
        PyObject *tmp_key_name_4;
        tmp_key_name_3 = const_str_plain___metaclass__;
        CHECK_OBJECT(tmp_class_creation_2__class_dict);
        tmp_dict_name_3 = tmp_class_creation_2__class_dict;
        tmp_res = PyDict_Contains(tmp_dict_name_3, tmp_key_name_3);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 38;

            goto try_except_handler_7;
        }
        tmp_condition_result_3 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
            goto condexpr_true_2;
        } else {
            goto condexpr_false_2;
        }
        condexpr_true_2:;
        CHECK_OBJECT(tmp_class_creation_2__class_dict);
        tmp_dict_name_4 = tmp_class_creation_2__class_dict;
        tmp_key_name_4 = const_str_plain___metaclass__;
        tmp_assign_source_20 = DICT_GET_ITEM(tmp_dict_name_4, tmp_key_name_4);
        if (tmp_assign_source_20 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 38;

            goto try_except_handler_7;
        }
        goto condexpr_end_2;
        condexpr_false_2:;
        {
            PyObject *tmp_assign_source_21;
            PyObject *tmp_subscribed_name_2;
            PyObject *tmp_subscript_name_2;
            CHECK_OBJECT(tmp_class_creation_2__bases);
            tmp_subscribed_name_2 = tmp_class_creation_2__bases;
            tmp_subscript_name_2 = const_int_0;
            tmp_assign_source_21 = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_2, tmp_subscript_name_2, 0);
            if (tmp_assign_source_21 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 38;

                goto try_except_handler_7;
            }
            assert(tmp_select_metaclass_2__base == NULL);
            tmp_select_metaclass_2__base = tmp_assign_source_21;
        }
        // Tried code:
        // Tried code:
        {
            PyObject *tmp_source_name_3;
            CHECK_OBJECT(tmp_select_metaclass_2__base);
            tmp_source_name_3 = tmp_select_metaclass_2__base;
            tmp_assign_source_20 = LOOKUP_ATTRIBUTE_CLASS_SLOT(tmp_source_name_3);
            if (tmp_assign_source_20 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 38;

                goto try_except_handler_10;
            }
            goto try_return_handler_9;
        }
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Exception handler code:
        try_except_handler_10:;
        exception_keeper_type_7 = exception_type;
        exception_keeper_value_7 = exception_value;
        exception_keeper_tb_7 = exception_tb;
        exception_keeper_lineno_7 = exception_lineno;
        exception_type = NULL;
        exception_value = NULL;
        exception_tb = NULL;
        exception_lineno = 0;

        {
            PyObject *tmp_type_arg_2;
            Py_DECREF(exception_keeper_type_7);
            Py_XDECREF(exception_keeper_value_7);
            Py_XDECREF(exception_keeper_tb_7);
            CHECK_OBJECT(tmp_select_metaclass_2__base);
            tmp_type_arg_2 = tmp_select_metaclass_2__base;
            tmp_assign_source_20 = BUILTIN_TYPE1(tmp_type_arg_2);
            assert(!(tmp_assign_source_20 == NULL));
            goto try_return_handler_9;
        }
        // End of try:
        // tried codes exits in all cases
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        // Return handler code:
        try_return_handler_9:;
        CHECK_OBJECT((PyObject *)tmp_select_metaclass_2__base);
        Py_DECREF(tmp_select_metaclass_2__base);
        tmp_select_metaclass_2__base = NULL;

        goto outline_result_4;
        // End of try:
        CHECK_OBJECT((PyObject *)tmp_select_metaclass_2__base);
        Py_DECREF(tmp_select_metaclass_2__base);
        tmp_select_metaclass_2__base = NULL;

        // Return statement must have exited already.
        NUITKA_CANNOT_GET_HERE(gio);
        return NULL;
        outline_result_4:;
        condexpr_end_2:;
        assert(tmp_class_creation_2__metaclass == NULL);
        tmp_class_creation_2__metaclass = tmp_assign_source_20;
    }
    {
        PyObject *tmp_assign_source_22;
        PyObject *tmp_called_name_4;
        PyObject *tmp_args_element_name_6;
        PyObject *tmp_args_element_name_7;
        PyObject *tmp_args_element_name_8;
        CHECK_OBJECT(tmp_class_creation_2__metaclass);
        tmp_called_name_4 = tmp_class_creation_2__metaclass;
        tmp_args_element_name_6 = const_str_plain_GAppInfoMeta;
        CHECK_OBJECT(tmp_class_creation_2__bases);
        tmp_args_element_name_7 = tmp_class_creation_2__bases;
        CHECK_OBJECT(tmp_class_creation_2__class_dict);
        tmp_args_element_name_8 = tmp_class_creation_2__class_dict;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 38;
        {
            PyObject *call_args[] = {tmp_args_element_name_6, tmp_args_element_name_7, tmp_args_element_name_8};
            tmp_assign_source_22 = CALL_FUNCTION_WITH_ARGS3(tmp_called_name_4, call_args);
        }

        if (tmp_assign_source_22 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 38;

            goto try_except_handler_7;
        }
        assert(tmp_class_creation_2__class == NULL);
        tmp_class_creation_2__class = tmp_assign_source_22;
    }
    goto try_end_4;
    // Exception handler code:
    try_except_handler_7:;
    exception_keeper_type_8 = exception_type;
    exception_keeper_value_8 = exception_value;
    exception_keeper_tb_8 = exception_tb;
    exception_keeper_lineno_8 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(tmp_class_creation_2__bases);
    tmp_class_creation_2__bases = NULL;

    Py_XDECREF(tmp_class_creation_2__class_dict);
    tmp_class_creation_2__class_dict = NULL;

    Py_XDECREF(tmp_class_creation_2__metaclass);
    tmp_class_creation_2__metaclass = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_8;
    exception_value = exception_keeper_value_8;
    exception_tb = exception_keeper_tb_8;
    exception_lineno = exception_keeper_lineno_8;

    goto frame_exception_exit_1;
    // End of try:
    try_end_4:;
    {
        PyObject *tmp_assign_source_23;
        CHECK_OBJECT(tmp_class_creation_2__class);
        tmp_assign_source_23 = tmp_class_creation_2__class;
        UPDATE_STRING_DICT0(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GAppInfoMeta, tmp_assign_source_23);
    }
    CHECK_OBJECT((PyObject *)tmp_class_creation_2__class);
    Py_DECREF(tmp_class_creation_2__class);
    tmp_class_creation_2__class = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_2__bases);
    Py_DECREF(tmp_class_creation_2__bases);
    tmp_class_creation_2__bases = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_2__class_dict);
    Py_DECREF(tmp_class_creation_2__class_dict);
    tmp_class_creation_2__class_dict = NULL;

    CHECK_OBJECT((PyObject *)tmp_class_creation_2__metaclass);
    Py_DECREF(tmp_class_creation_2__metaclass);
    tmp_class_creation_2__metaclass = NULL;

    {
        PyObject *tmp_called_name_5;
        PyObject *tmp_mvar_value_9;
        PyObject *tmp_call_result_2;
        PyObject *tmp_args_element_name_9;
        PyObject *tmp_mvar_value_10;
        tmp_mvar_value_9 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain__install_app_info_meta);

        if (unlikely(tmp_mvar_value_9 == NULL)) {
            tmp_mvar_value_9 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__install_app_info_meta);
        }

        if (tmp_mvar_value_9 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1827 ], 44, 0);
            exception_tb = NULL;

            exception_lineno = 40;

            goto frame_exception_exit_1;
        }

        tmp_called_name_5 = tmp_mvar_value_9;
        tmp_mvar_value_10 = GET_STRING_DICT_VALUE(moduledict_gio, (Nuitka_StringObject *)const_str_plain_GAppInfoMeta);

        if (unlikely(tmp_mvar_value_10 == NULL)) {
            tmp_mvar_value_10 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_GAppInfoMeta);
        }

        if (tmp_mvar_value_10 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 1871 ], 34, 0);
            exception_tb = NULL;

            exception_lineno = 40;

            goto frame_exception_exit_1;
        }

        tmp_args_element_name_9 = tmp_mvar_value_10;
        frame_0eaf5c8946efac81ec709789545b817d->m_frame.f_lineno = 40;
        tmp_call_result_2 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_5, tmp_args_element_name_9);
        if (tmp_call_result_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 40;

            goto frame_exception_exit_1;
        }
        Py_DECREF(tmp_call_result_2);
    }

    // Restore frame exception if necessary.
#if 1
    RESTORE_FRAME_EXCEPTION(frame_0eaf5c8946efac81ec709789545b817d);
#endif
    popFrameStack();

    assertFrameObject(frame_0eaf5c8946efac81ec709789545b817d);

    goto frame_no_exception_3;

    frame_exception_exit_1:;
#if 1
    RESTORE_FRAME_EXCEPTION(frame_0eaf5c8946efac81ec709789545b817d);
#endif

    if ( exception_tb == NULL )
    {
        exception_tb = MAKE_TRACEBACK(frame_0eaf5c8946efac81ec709789545b817d, exception_lineno);
    }
    else if (exception_tb->tb_frame != &frame_0eaf5c8946efac81ec709789545b817d->m_frame) {
        exception_tb = ADD_TRACEBACK(exception_tb, frame_0eaf5c8946efac81ec709789545b817d, exception_lineno);
    }

    // Put the previous frame back on top.
    popFrameStack();

    // Return the error.
    goto module_exception_exit;

    frame_no_exception_3:;

    return module_gio;
    module_exception_exit:
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);
    return NULL;
}
