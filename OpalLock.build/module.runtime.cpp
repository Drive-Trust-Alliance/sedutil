/* Generated code for Python module 'runtime'
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

/* The "_module_runtime" is a Python object pointer of module type.
 *
 * Note: For full compatibility with CPython, every module variable access
 * needs to go through it except for cases where the module cannot possibly
 * have changed in the mean time.
 */

PyObject *module_runtime;
PyDictObject *moduledict_runtime;

/* The declarations of module constants used, if any. */
static PyObject *const_str_plain_verbose;
static PyObject *const_str_plain_bin;
static PyObject *const_str_plain_flush;
extern PyObject *const_str_plain_value;
static PyObject *const_str_digest_bc00a84b40c409ab8d40469dc39b0aac;
static PyObject *const_str_plain__putenv;
extern PyObject *const_str_plain_get;
extern PyObject *const_str_digest_5bfaf90dbd407b4fc29090c8f6415242;
extern PyObject *const_str_plain_write;
static PyObject *const_tuple_str_plain_find_msvcrt_tuple;
extern PyObject *const_str_dot;
extern PyObject *const_str_plain_insert;
static PyObject *const_tuple_3911f6818934bf315c20b2d1c4839f08_tuple;
static PyObject *const_tuple_str_plain_cdll_tuple;
extern PyObject *const_str_plain_Warning;
static PyObject *const_str_plain_LoadLibrary;
static PyObject *const_str_digest_368a9f60f53f150afb585a023783bd94;
static PyObject *const_tuple_str_digest_0b7188faf47a950049115f24f617551b_tuple;
static PyObject *const_str_plain_pathsep;
extern PyObject *const_str_plain_win32;
static PyObject *const_tuple_str_dot_tuple;
static PyObject *const_str_digest_db4a5a786e788512167378827f558f95;
static PyObject *const_str_digest_25ea7f10d2ebb0c8b282b6c60bd61617;
extern PyObject *const_str_plain_split;
extern PyObject *const_str_plain_path;
static PyObject *const_tuple_str_digest_70f1ee9ca166e607f11166ae647e026d_tuple;
static PyObject *const_str_digest_1e673c36590ff286a96617a71877a90e;
extern PyObject *const_str_plain_os;
extern PyObject *const_str_plain___doc__;
extern PyObject *const_str_plain_sys;
static PyObject *const_str_plain_msvcrt;
extern PyObject *const_str_plain_ctypes;
static PyObject *const_str_plain_SetEnvironmentVariableW;
extern PyObject *const_int_0;
extern PyObject *const_tuple_empty;
static PyObject *const_tuple_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce_tuple;
static PyObject *const_tuple_str_plain_windll_tuple;
extern PyObject *const_str_plain_kernel32;
extern PyObject *const_str_plain_platform;
static PyObject *const_str_plain_NUITKA_PACKAGE_runtime;
static PyObject *const_tuple_192607ca7d378761e68ef323d58e582b_tuple;
static PyObject *const_str_plain_stderr;
extern PyObject *const_str_plain_join;
static PyObject *const_str_digest_81daf1bf176566b0ca1361c8d199a6c0;
static PyObject *const_str_plain_result;
static PyObject *const_str_digest_70f1ee9ca166e607f11166ae647e026d;
static PyObject *const_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce;
static PyObject *const_str_digest_7f77dec5c33ddaf396cffb1d0942df13;
static PyObject *const_str_digest_0b7188faf47a950049115f24f617551b;
static PyObject *const_str_plain_cdll;
static PyObject *const_str_plain_ABSPATH;
extern PyObject *const_str_plain___path__;
static PyObject *const_str_digest_a2d9fabdc40336956ec28912a4fd7d01;
static PyObject *const_str_plain_abspath;
static PyObject *const_tuple_str_digest_db4a5a786e788512167378827f558f95_tuple;
static PyObject *const_str_plain_RUNTIME;
static PyObject *const_str_digest_6b798ecb2bcbad52c644b98773ac8df3;
extern PyObject *const_str_plain_flags;
extern PyObject *const_str_plain___file__;
static PyObject *const_str_plain_PATH;
static PyObject *const_str_plain_msvcrtname;
extern PyObject *const_str_plain_environ;
extern PyObject *const_str_plain_windll;
static PyObject *const_str_plain_x;
extern PyObject *const_str_plain_name;
static PyObject *const_str_digest_581e9157f4cfd7a80dd5ba063afd246e;
static PyObject *const_str_plain_find_msvcrt;
static PyObject *const_str_digest_85f5b38989fab5199ef6cd794aeca882;
static PyObject *const_str_digest_d26f52d432ccf199e53ad3ddfa46aa69;
extern PyObject *const_str_plain_dirname;
static PyObject *const_str_plain_nt;
static PyObject *module_filename_obj;

/* Indicator if this modules private constants were created yet. */
static bool constants_created = false;

/* Function to create module private constants. */
static void createModuleConstants(void) {
    const_str_plain_verbose = UNSTREAM_STRING(&constant_bin[ 45337 ], 7, 1);
    const_str_plain_bin = UNSTREAM_STRING(&constant_bin[ 45344 ], 3, 1);
    const_str_plain_flush = UNSTREAM_STRING(&constant_bin[ 45347 ], 5, 1);
    const_str_digest_bc00a84b40c409ab8d40469dc39b0aac = UNSTREAM_STRING(&constant_bin[ 45352 ], 41, 0);
    const_str_plain__putenv = UNSTREAM_STRING(&constant_bin[ 5465 ], 7, 1);
    const_tuple_str_plain_find_msvcrt_tuple = PyTuple_New(1);
    const_str_plain_find_msvcrt = UNSTREAM_STRING(&constant_bin[ 5324 ], 11, 1);
    PyTuple_SET_ITEM(const_tuple_str_plain_find_msvcrt_tuple, 0, const_str_plain_find_msvcrt); Py_INCREF(const_str_plain_find_msvcrt);
    const_tuple_3911f6818934bf315c20b2d1c4839f08_tuple = PyTuple_New(2);
    const_str_plain_NUITKA_PACKAGE_runtime = UNSTREAM_STRING(&constant_bin[ 45393 ], 22, 1);
    PyTuple_SET_ITEM(const_tuple_3911f6818934bf315c20b2d1c4839f08_tuple, 0, const_str_plain_NUITKA_PACKAGE_runtime); Py_INCREF(const_str_plain_NUITKA_PACKAGE_runtime);
    PyTuple_SET_ITEM(const_tuple_3911f6818934bf315c20b2d1c4839f08_tuple, 1, const_str_digest_5bfaf90dbd407b4fc29090c8f6415242); Py_INCREF(const_str_digest_5bfaf90dbd407b4fc29090c8f6415242);
    const_tuple_str_plain_cdll_tuple = PyTuple_New(1);
    const_str_plain_cdll = UNSTREAM_STRING(&constant_bin[ 5291 ], 4, 1);
    PyTuple_SET_ITEM(const_tuple_str_plain_cdll_tuple, 0, const_str_plain_cdll); Py_INCREF(const_str_plain_cdll);
    const_str_plain_LoadLibrary = UNSTREAM_STRING(&constant_bin[ 45415 ], 11, 1);
    const_str_digest_368a9f60f53f150afb585a023783bd94 = UNSTREAM_STRING(&constant_bin[ 45426 ], 36, 0);
    const_tuple_str_digest_0b7188faf47a950049115f24f617551b_tuple = PyTuple_New(1);
    const_str_digest_0b7188faf47a950049115f24f617551b = UNSTREAM_STRING(&constant_bin[ 45462 ], 63, 0);
    PyTuple_SET_ITEM(const_tuple_str_digest_0b7188faf47a950049115f24f617551b_tuple, 0, const_str_digest_0b7188faf47a950049115f24f617551b); Py_INCREF(const_str_digest_0b7188faf47a950049115f24f617551b);
    const_str_plain_pathsep = UNSTREAM_STRING(&constant_bin[ 45525 ], 7, 1);
    const_tuple_str_dot_tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(const_tuple_str_dot_tuple, 0, const_str_dot); Py_INCREF(const_str_dot);
    const_str_digest_db4a5a786e788512167378827f558f95 = UNSTREAM_STRING(&constant_bin[ 45532 ], 41, 0);
    const_str_digest_25ea7f10d2ebb0c8b282b6c60bd61617 = UNSTREAM_STRING(&constant_bin[ 45573 ], 37, 0);
    const_tuple_str_digest_70f1ee9ca166e607f11166ae647e026d_tuple = PyTuple_New(1);
    const_str_digest_70f1ee9ca166e607f11166ae647e026d = UNSTREAM_STRING(&constant_bin[ 45610 ], 45, 0);
    PyTuple_SET_ITEM(const_tuple_str_digest_70f1ee9ca166e607f11166ae647e026d_tuple, 0, const_str_digest_70f1ee9ca166e607f11166ae647e026d); Py_INCREF(const_str_digest_70f1ee9ca166e607f11166ae647e026d);
    const_str_digest_1e673c36590ff286a96617a71877a90e = UNSTREAM_STRING(&constant_bin[ 45655 ], 38, 0);
    const_str_plain_msvcrt = UNSTREAM_STRING(&constant_bin[ 5329 ], 6, 1);
    const_str_plain_SetEnvironmentVariableW = UNSTREAM_STRING(&constant_bin[ 45489 ], 23, 1);
    const_tuple_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce_tuple = PyTuple_New(1);
    const_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce = UNSTREAM_STRING(&constant_bin[ 45693 ], 59, 0);
    PyTuple_SET_ITEM(const_tuple_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce_tuple, 0, const_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce); Py_INCREF(const_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce);
    const_tuple_str_plain_windll_tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(const_tuple_str_plain_windll_tuple, 0, const_str_plain_windll); Py_INCREF(const_str_plain_windll);
    const_tuple_192607ca7d378761e68ef323d58e582b_tuple = PyTuple_New(5);
    PyTuple_SET_ITEM(const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 0, const_str_plain_name); Py_INCREF(const_str_plain_name);
    PyTuple_SET_ITEM(const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 1, const_str_plain_value); Py_INCREF(const_str_plain_value);
    const_str_plain_msvcrtname = UNSTREAM_STRING(&constant_bin[ 45752 ], 10, 1);
    PyTuple_SET_ITEM(const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 2, const_str_plain_msvcrtname); Py_INCREF(const_str_plain_msvcrtname);
    PyTuple_SET_ITEM(const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 3, const_str_plain_msvcrt); Py_INCREF(const_str_plain_msvcrt);
    const_str_plain_result = UNSTREAM_STRING(&constant_bin[ 28347 ], 6, 1);
    PyTuple_SET_ITEM(const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 4, const_str_plain_result); Py_INCREF(const_str_plain_result);
    const_str_plain_stderr = UNSTREAM_STRING(&constant_bin[ 45762 ], 6, 1);
    const_str_digest_81daf1bf176566b0ca1361c8d199a6c0 = UNSTREAM_STRING(&constant_bin[ 45768 ], 41, 0);
    const_str_digest_7f77dec5c33ddaf396cffb1d0942df13 = UNSTREAM_STRING(&constant_bin[ 45809 ], 16, 0);
    const_str_plain_ABSPATH = UNSTREAM_STRING(&constant_bin[ 45825 ], 7, 1);
    const_str_digest_a2d9fabdc40336956ec28912a4fd7d01 = UNSTREAM_STRING(&constant_bin[ 45832 ], 36, 0);
    const_str_plain_abspath = UNSTREAM_STRING(&constant_bin[ 45868 ], 7, 1);
    const_tuple_str_digest_db4a5a786e788512167378827f558f95_tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(const_tuple_str_digest_db4a5a786e788512167378827f558f95_tuple, 0, const_str_digest_db4a5a786e788512167378827f558f95); Py_INCREF(const_str_digest_db4a5a786e788512167378827f558f95);
    const_str_plain_RUNTIME = UNSTREAM_STRING(&constant_bin[ 5410 ], 7, 1);
    const_str_digest_6b798ecb2bcbad52c644b98773ac8df3 = UNSTREAM_STRING(&constant_bin[ 45875 ], 19, 0);
    const_str_plain_PATH = UNSTREAM_STRING(&constant_bin[ 5439 ], 4, 1);
    const_str_plain_x = UNSTREAM_CHAR(120, 1);
    const_str_digest_581e9157f4cfd7a80dd5ba063afd246e = UNSTREAM_STRING(&constant_bin[ 45894 ], 11, 0);
    const_str_digest_85f5b38989fab5199ef6cd794aeca882 = UNSTREAM_STRING(&constant_bin[ 45905 ], 910, 0);
    const_str_digest_d26f52d432ccf199e53ad3ddfa46aa69 = UNSTREAM_STRING(&constant_bin[ 46815 ], 5, 0);
    const_str_plain_nt = UNSTREAM_STRING(&constant_bin[ 1286 ], 2, 1);

    constants_created = true;
}

/* Function to verify module private constants for non-corruption. */
#ifndef __NUITKA_NO_ASSERT__
void checkModuleConstants_runtime(void) {
    // The module may not have been used at all, then ignore this.
    if (constants_created == false) return;


}
#endif

// The module code objects.
static PyCodeObject *codeobj_a80a886d96f093ab3a7b5df1f97de909;
static PyCodeObject *codeobj_be1aab7b1485b4d13183f2905837edc4;

static void createModuleCodeObjects(void) {
    module_filename_obj = MAKE_RELATIVE_PATH(const_str_digest_6b798ecb2bcbad52c644b98773ac8df3);
    codeobj_a80a886d96f093ab3a7b5df1f97de909 = MAKE_CODEOBJECT(module_filename_obj, 1, CO_NOFREE, const_str_digest_7f77dec5c33ddaf396cffb1d0942df13, const_tuple_empty, 0, 0, 0);
    codeobj_be1aab7b1485b4d13183f2905837edc4 = MAKE_CODEOBJECT(module_filename_obj, 12, CO_OPTIMIZED | CO_NEWLOCALS | CO_NOFREE, const_str_plain__putenv, const_tuple_192607ca7d378761e68ef323d58e582b_tuple, 2, 0, 0);
}

// The module function declarations.
static PyObject *MAKE_FUNCTION_runtime$$$function_1__putenv();


// The module function definitions.
static PyObject *impl_runtime$$$function_1__putenv(struct Nuitka_FunctionObject const *self, PyObject **python_pars) {
    // Preserve error status for checks
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
#endif

    // Local variable declarations.
    PyObject *par_name = python_pars[0];
    PyObject *par_value = python_pars[1];
    PyObject *var_msvcrtname = NULL;
    PyObject *var_msvcrt = NULL;
    PyObject *var_result = NULL;
    nuitka_bool tmp_try_except_1__unhandled_indicator = NUITKA_BOOL_UNASSIGNED;
    nuitka_bool tmp_try_except_2__unhandled_indicator = NUITKA_BOOL_UNASSIGNED;
    nuitka_bool tmp_try_except_3__unhandled_indicator = NUITKA_BOOL_UNASSIGNED;
    struct Nuitka_FrameObject *frame_be1aab7b1485b4d13183f2905837edc4;
    NUITKA_MAY_BE_UNUSED char const *type_description_1 = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    bool tmp_result;
    int tmp_res;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
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
    PyObject *exception_keeper_type_6;
    PyObject *exception_keeper_value_6;
    PyTracebackObject *exception_keeper_tb_6;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_6;
    static struct Nuitka_FrameObject *cache_frame_be1aab7b1485b4d13183f2905837edc4 = NULL;
    PyObject *tmp_return_value = NULL;
    PyObject *exception_keeper_type_7;
    PyObject *exception_keeper_value_7;
    PyTracebackObject *exception_keeper_tb_7;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_7;

    // Actual function body.
    // Tried code:
    MAKE_OR_REUSE_FRAME(cache_frame_be1aab7b1485b4d13183f2905837edc4, codeobj_be1aab7b1485b4d13183f2905837edc4, module_runtime, sizeof(void *)+sizeof(void *)+sizeof(void *)+sizeof(void *)+sizeof(void *));
    frame_be1aab7b1485b4d13183f2905837edc4 = cache_frame_be1aab7b1485b4d13183f2905837edc4;

    // Push the new frame as the currently active one.
    pushFrameStack(frame_be1aab7b1485b4d13183f2905837edc4);

    // Mark the frame object as in use, ref count 1 will be up for reuse.
    assert(Py_REFCNT(frame_be1aab7b1485b4d13183f2905837edc4) == 2); // Frame stack

    // Framed code:
    {
        PyObject *tmp_ass_subvalue_1;
        PyObject *tmp_ass_subscribed_1;
        PyObject *tmp_source_name_1;
        PyObject *tmp_mvar_value_1;
        PyObject *tmp_ass_subscript_1;
        CHECK_OBJECT(par_value);
        tmp_ass_subvalue_1 = par_value;
        tmp_mvar_value_1 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

        if (unlikely(tmp_mvar_value_1 == NULL)) {
            tmp_mvar_value_1 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
        }

        if (tmp_mvar_value_1 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 348 ], 31, 0);
            exception_tb = NULL;

            exception_lineno = 32;
            type_description_1 = "ooooo";
            goto frame_exception_exit_1;
        }

        tmp_source_name_1 = tmp_mvar_value_1;
        tmp_ass_subscribed_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain_environ);
        if (tmp_ass_subscribed_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 32;
            type_description_1 = "ooooo";
            goto frame_exception_exit_1;
        }
        CHECK_OBJECT(par_name);
        tmp_ass_subscript_1 = par_name;
        tmp_result = SET_SUBSCRIPT(tmp_ass_subscribed_1, tmp_ass_subscript_1, tmp_ass_subvalue_1);
        Py_DECREF(tmp_ass_subscribed_1);
        if (tmp_result == false) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 32;
            type_description_1 = "ooooo";
            goto frame_exception_exit_1;
        }
    }
    {
        nuitka_bool tmp_assign_source_1;
        tmp_assign_source_1 = NUITKA_BOOL_TRUE;
        tmp_try_except_1__unhandled_indicator = tmp_assign_source_1;
    }
    // Tried code:
    // Tried code:
    {
        PyObject *tmp_assign_source_2;
        PyObject *tmp_called_instance_1;
        PyObject *tmp_source_name_2;
        PyObject *tmp_mvar_value_2;
        PyObject *tmp_args_element_name_1;
        PyObject *tmp_args_element_name_2;
        tmp_mvar_value_2 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_windll);

        if (unlikely(tmp_mvar_value_2 == NULL)) {
            tmp_mvar_value_2 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_windll);
        }

        if (tmp_mvar_value_2 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 5207 ], 35, 0);
            exception_tb = NULL;

            exception_lineno = 36;
            type_description_1 = "ooooo";
            goto try_except_handler_3;
        }

        tmp_source_name_2 = tmp_mvar_value_2;
        tmp_called_instance_1 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain_kernel32);
        if (tmp_called_instance_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 36;
            type_description_1 = "ooooo";
            goto try_except_handler_3;
        }
        CHECK_OBJECT(par_name);
        tmp_args_element_name_1 = par_name;
        CHECK_OBJECT(par_value);
        tmp_args_element_name_2 = par_value;
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 36;
        {
            PyObject *call_args[] = {tmp_args_element_name_1, tmp_args_element_name_2};
            tmp_assign_source_2 = CALL_METHOD_WITH_ARGS2(tmp_called_instance_1, const_str_plain_SetEnvironmentVariableW, call_args);
        }

        Py_DECREF(tmp_called_instance_1);
        if (tmp_assign_source_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 36;
            type_description_1 = "ooooo";
            goto try_except_handler_3;
        }
        assert(var_result == NULL);
        var_result = tmp_assign_source_2;
    }
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        CHECK_OBJECT(var_result);
        tmp_compexpr_left_1 = var_result;
        tmp_compexpr_right_1 = const_int_0;
        tmp_res = RICH_COMPARE_BOOL_EQ_OBJECT_INT(tmp_compexpr_left_1, tmp_compexpr_right_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 37;
            type_description_1 = "ooooo";
            goto try_except_handler_3;
        }
        tmp_condition_result_1 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        {
            PyObject *tmp_raise_type_1;
            PyObject *tmp_mvar_value_3;
            tmp_mvar_value_3 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_Warning);

            if (unlikely(tmp_mvar_value_3 == NULL)) {
                tmp_mvar_value_3 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_Warning);
            }

            if (tmp_mvar_value_3 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 5242 ], 36, 0);
                exception_tb = NULL;

                exception_lineno = 37;
                type_description_1 = "ooooo";
                goto try_except_handler_3;
            }

            tmp_raise_type_1 = tmp_mvar_value_3;
            exception_type = tmp_raise_type_1;
            Py_INCREF(tmp_raise_type_1);
            exception_lineno = 37;
            RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);
            type_description_1 = "ooooo";
            goto try_except_handler_3;
        }
        branch_no_1:;
    }
    goto try_end_1;
    // Exception handler code:
    try_except_handler_3:;
    exception_keeper_type_1 = exception_type;
    exception_keeper_value_1 = exception_value;
    exception_keeper_tb_1 = exception_tb;
    exception_keeper_lineno_1 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    {
        nuitka_bool tmp_assign_source_3;
        tmp_assign_source_3 = NUITKA_BOOL_FALSE;
        tmp_try_except_1__unhandled_indicator = tmp_assign_source_3;
    }
    // Preserve existing published exception.
    PRESERVE_FRAME_EXCEPTION(frame_be1aab7b1485b4d13183f2905837edc4);
    if (exception_keeper_tb_1 == NULL) {
        exception_keeper_tb_1 = MAKE_TRACEBACK(frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_1);
    } else if (exception_keeper_lineno_1 != 0) {
        exception_keeper_tb_1 = ADD_TRACEBACK(exception_keeper_tb_1, frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_1);
    }

    NORMALIZE_EXCEPTION(&exception_keeper_type_1, &exception_keeper_value_1, &exception_keeper_tb_1);
    PUBLISH_EXCEPTION(&exception_keeper_type_1, &exception_keeper_value_1, &exception_keeper_tb_1);
    {
        nuitka_bool tmp_condition_result_2;
        PyObject *tmp_compexpr_left_2;
        PyObject *tmp_compexpr_right_2;
        tmp_compexpr_left_2 = EXC_TYPE(PyThreadState_GET());
        tmp_compexpr_right_2 = PyExc_Exception;
        tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_2, tmp_compexpr_right_2);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 38;
            type_description_1 = "ooooo";
            goto try_except_handler_2;
        }
        tmp_condition_result_2 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_2 == NUITKA_BOOL_TRUE) {
            goto branch_yes_2;
        } else {
            goto branch_no_2;
        }
        branch_yes_2:;
        {
            nuitka_bool tmp_condition_result_3;
            PyObject *tmp_source_name_3;
            PyObject *tmp_source_name_4;
            PyObject *tmp_mvar_value_4;
            PyObject *tmp_attribute_value_1;
            int tmp_truth_name_1;
            tmp_mvar_value_4 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_4 == NULL)) {
                tmp_mvar_value_4 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_4 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 39;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }

            tmp_source_name_4 = tmp_mvar_value_4;
            tmp_source_name_3 = LOOKUP_ATTRIBUTE(tmp_source_name_4, const_str_plain_flags);
            if (tmp_source_name_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 39;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_attribute_value_1 = LOOKUP_ATTRIBUTE(tmp_source_name_3, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_3);
            if (tmp_attribute_value_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 39;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_truth_name_1 = CHECK_IF_TRUE(tmp_attribute_value_1);
            if (tmp_truth_name_1 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_1);

                exception_lineno = 39;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_condition_result_3 = tmp_truth_name_1 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_1);
            if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
                goto branch_yes_3;
            } else {
                goto branch_no_3;
            }
            branch_yes_3:;
            {
                PyObject *tmp_called_instance_2;
                PyObject *tmp_source_name_5;
                PyObject *tmp_mvar_value_5;
                PyObject *tmp_call_result_1;
                tmp_mvar_value_5 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_5 == NULL)) {
                    tmp_mvar_value_5 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_5 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 40;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }

                tmp_source_name_5 = tmp_mvar_value_5;
                tmp_called_instance_2 = LOOKUP_ATTRIBUTE(tmp_source_name_5, const_str_plain_stderr);
                if (tmp_called_instance_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 40;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 40;
                tmp_call_result_1 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_2, const_str_plain_write, &PyTuple_GET_ITEM(const_tuple_str_digest_5edd30b89a9faebbe36bd86cbcc1a4ce_tuple, 0));

                Py_DECREF(tmp_called_instance_2);
                if (tmp_call_result_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 40;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                Py_DECREF(tmp_call_result_1);
            }
            {
                PyObject *tmp_called_instance_3;
                PyObject *tmp_source_name_6;
                PyObject *tmp_mvar_value_6;
                PyObject *tmp_call_result_2;
                tmp_mvar_value_6 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_6 == NULL)) {
                    tmp_mvar_value_6 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_6 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 41;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }

                tmp_source_name_6 = tmp_mvar_value_6;
                tmp_called_instance_3 = LOOKUP_ATTRIBUTE(tmp_source_name_6, const_str_plain_stderr);
                if (tmp_called_instance_3 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 41;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 41;
                tmp_call_result_2 = CALL_METHOD_NO_ARGS(tmp_called_instance_3, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_3);
                if (tmp_call_result_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 41;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                Py_DECREF(tmp_call_result_2);
            }
            branch_no_3:;
        }
        goto branch_end_2;
        branch_no_2:;
        tmp_result = RERAISE_EXCEPTION(&exception_type, &exception_value, &exception_tb);
        if (unlikely(tmp_result == false)) {
            exception_lineno = 35;
        }

        if (exception_tb && exception_tb->tb_frame == &frame_be1aab7b1485b4d13183f2905837edc4->m_frame) frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = exception_tb->tb_lineno;
        type_description_1 = "ooooo";
        goto try_except_handler_2;
        branch_end_2:;
    }
    goto try_end_1;
    // exception handler codes exits in all cases
    NUITKA_CANNOT_GET_HERE(runtime$$$function_1__putenv);
    return NULL;
    // End of try:
    try_end_1:;
    {
        nuitka_bool tmp_condition_result_4;
        nuitka_bool tmp_compexpr_left_3;
        nuitka_bool tmp_compexpr_right_3;
        assert(tmp_try_except_1__unhandled_indicator != NUITKA_BOOL_UNASSIGNED);
        tmp_compexpr_left_3 = tmp_try_except_1__unhandled_indicator;
        tmp_compexpr_right_3 = NUITKA_BOOL_TRUE;
        tmp_condition_result_4 = (tmp_compexpr_left_3 == tmp_compexpr_right_3) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_4 == NUITKA_BOOL_TRUE) {
            goto branch_yes_4;
        } else {
            goto branch_no_4;
        }
        branch_yes_4:;
        {
            nuitka_bool tmp_condition_result_5;
            PyObject *tmp_source_name_7;
            PyObject *tmp_source_name_8;
            PyObject *tmp_mvar_value_7;
            PyObject *tmp_attribute_value_2;
            int tmp_truth_name_2;
            tmp_mvar_value_7 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_7 == NULL)) {
                tmp_mvar_value_7 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_7 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 43;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }

            tmp_source_name_8 = tmp_mvar_value_7;
            tmp_source_name_7 = LOOKUP_ATTRIBUTE(tmp_source_name_8, const_str_plain_flags);
            if (tmp_source_name_7 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 43;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_attribute_value_2 = LOOKUP_ATTRIBUTE(tmp_source_name_7, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_7);
            if (tmp_attribute_value_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 43;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_truth_name_2 = CHECK_IF_TRUE(tmp_attribute_value_2);
            if (tmp_truth_name_2 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_2);

                exception_lineno = 43;
                type_description_1 = "ooooo";
                goto try_except_handler_2;
            }
            tmp_condition_result_5 = tmp_truth_name_2 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_2);
            if (tmp_condition_result_5 == NUITKA_BOOL_TRUE) {
                goto branch_yes_5;
            } else {
                goto branch_no_5;
            }
            branch_yes_5:;
            {
                PyObject *tmp_called_instance_4;
                PyObject *tmp_source_name_9;
                PyObject *tmp_mvar_value_8;
                PyObject *tmp_call_result_3;
                tmp_mvar_value_8 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_8 == NULL)) {
                    tmp_mvar_value_8 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_8 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 44;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }

                tmp_source_name_9 = tmp_mvar_value_8;
                tmp_called_instance_4 = LOOKUP_ATTRIBUTE(tmp_source_name_9, const_str_plain_stderr);
                if (tmp_called_instance_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 44;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 44;
                tmp_call_result_3 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_4, const_str_plain_write, &PyTuple_GET_ITEM(const_tuple_str_digest_0b7188faf47a950049115f24f617551b_tuple, 0));

                Py_DECREF(tmp_called_instance_4);
                if (tmp_call_result_3 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 44;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                Py_DECREF(tmp_call_result_3);
            }
            {
                PyObject *tmp_called_instance_5;
                PyObject *tmp_source_name_10;
                PyObject *tmp_mvar_value_9;
                PyObject *tmp_call_result_4;
                tmp_mvar_value_9 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_9 == NULL)) {
                    tmp_mvar_value_9 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_9 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 45;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }

                tmp_source_name_10 = tmp_mvar_value_9;
                tmp_called_instance_5 = LOOKUP_ATTRIBUTE(tmp_source_name_10, const_str_plain_stderr);
                if (tmp_called_instance_5 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 45;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 45;
                tmp_call_result_4 = CALL_METHOD_NO_ARGS(tmp_called_instance_5, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_5);
                if (tmp_call_result_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 45;
                    type_description_1 = "ooooo";
                    goto try_except_handler_2;
                }
                Py_DECREF(tmp_call_result_4);
            }
            branch_no_5:;
        }
        branch_no_4:;
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

    // Re-raise.
    exception_type = exception_keeper_type_2;
    exception_value = exception_keeper_value_2;
    exception_tb = exception_keeper_tb_2;
    exception_lineno = exception_keeper_lineno_2;

    goto frame_exception_exit_1;
    // End of try:
    try_end_2:;
    {
        nuitka_bool tmp_assign_source_4;
        tmp_assign_source_4 = NUITKA_BOOL_TRUE;
        tmp_try_except_2__unhandled_indicator = tmp_assign_source_4;
    }
    // Tried code:
    // Tried code:
    {
        PyObject *tmp_assign_source_5;
        PyObject *tmp_called_name_1;
        PyObject *tmp_source_name_11;
        PyObject *tmp_source_name_12;
        PyObject *tmp_mvar_value_10;
        PyObject *tmp_args_element_name_3;
        PyObject *tmp_left_name_1;
        PyObject *tmp_right_name_1;
        PyObject *tmp_tuple_element_1;
        tmp_mvar_value_10 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_cdll);

        if (unlikely(tmp_mvar_value_10 == NULL)) {
            tmp_mvar_value_10 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_cdll);
        }

        if (tmp_mvar_value_10 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 5278 ], 33, 0);
            exception_tb = NULL;

            exception_lineno = 49;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }

        tmp_source_name_12 = tmp_mvar_value_10;
        tmp_source_name_11 = LOOKUP_ATTRIBUTE(tmp_source_name_12, const_str_plain_msvcrt);
        if (tmp_source_name_11 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 49;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        tmp_called_name_1 = LOOKUP_ATTRIBUTE(tmp_source_name_11, const_str_plain__putenv);
        Py_DECREF(tmp_source_name_11);
        if (tmp_called_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 49;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        tmp_left_name_1 = const_str_digest_d26f52d432ccf199e53ad3ddfa46aa69;
        CHECK_OBJECT(par_name);
        tmp_tuple_element_1 = par_name;
        tmp_right_name_1 = PyTuple_New(2);
        Py_INCREF(tmp_tuple_element_1);
        PyTuple_SET_ITEM(tmp_right_name_1, 0, tmp_tuple_element_1);
        CHECK_OBJECT(par_value);
        tmp_tuple_element_1 = par_value;
        Py_INCREF(tmp_tuple_element_1);
        PyTuple_SET_ITEM(tmp_right_name_1, 1, tmp_tuple_element_1);
        tmp_args_element_name_3 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_1, tmp_right_name_1);
        Py_DECREF(tmp_right_name_1);
        if (tmp_args_element_name_3 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_1);

            exception_lineno = 49;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 49;
        tmp_assign_source_5 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_1, tmp_args_element_name_3);
        Py_DECREF(tmp_called_name_1);
        Py_DECREF(tmp_args_element_name_3);
        if (tmp_assign_source_5 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 49;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        {
            PyObject *old = var_result;
            var_result = tmp_assign_source_5;
            Py_XDECREF(old);
        }

    }
    {
        nuitka_bool tmp_condition_result_6;
        PyObject *tmp_compexpr_left_4;
        PyObject *tmp_compexpr_right_4;
        CHECK_OBJECT(var_result);
        tmp_compexpr_left_4 = var_result;
        tmp_compexpr_right_4 = const_int_0;
        tmp_res = RICH_COMPARE_BOOL_NOTEQ_OBJECT_INT(tmp_compexpr_left_4, tmp_compexpr_right_4);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 50;
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        tmp_condition_result_6 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_6 == NUITKA_BOOL_TRUE) {
            goto branch_yes_6;
        } else {
            goto branch_no_6;
        }
        branch_yes_6:;
        {
            PyObject *tmp_raise_type_2;
            PyObject *tmp_mvar_value_11;
            tmp_mvar_value_11 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_Warning);

            if (unlikely(tmp_mvar_value_11 == NULL)) {
                tmp_mvar_value_11 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_Warning);
            }

            if (tmp_mvar_value_11 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 5242 ], 36, 0);
                exception_tb = NULL;

                exception_lineno = 50;
                type_description_1 = "ooooo";
                goto try_except_handler_5;
            }

            tmp_raise_type_2 = tmp_mvar_value_11;
            exception_type = tmp_raise_type_2;
            Py_INCREF(tmp_raise_type_2);
            exception_lineno = 50;
            RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);
            type_description_1 = "ooooo";
            goto try_except_handler_5;
        }
        branch_no_6:;
    }
    goto try_end_3;
    // Exception handler code:
    try_except_handler_5:;
    exception_keeper_type_3 = exception_type;
    exception_keeper_value_3 = exception_value;
    exception_keeper_tb_3 = exception_tb;
    exception_keeper_lineno_3 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    {
        nuitka_bool tmp_assign_source_6;
        tmp_assign_source_6 = NUITKA_BOOL_FALSE;
        tmp_try_except_2__unhandled_indicator = tmp_assign_source_6;
    }
    // Preserve existing published exception.
    PRESERVE_FRAME_EXCEPTION(frame_be1aab7b1485b4d13183f2905837edc4);
    if (exception_keeper_tb_3 == NULL) {
        exception_keeper_tb_3 = MAKE_TRACEBACK(frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_3);
    } else if (exception_keeper_lineno_3 != 0) {
        exception_keeper_tb_3 = ADD_TRACEBACK(exception_keeper_tb_3, frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_3);
    }

    NORMALIZE_EXCEPTION(&exception_keeper_type_3, &exception_keeper_value_3, &exception_keeper_tb_3);
    PUBLISH_EXCEPTION(&exception_keeper_type_3, &exception_keeper_value_3, &exception_keeper_tb_3);
    {
        nuitka_bool tmp_condition_result_7;
        PyObject *tmp_compexpr_left_5;
        PyObject *tmp_compexpr_right_5;
        tmp_compexpr_left_5 = EXC_TYPE(PyThreadState_GET());
        tmp_compexpr_right_5 = PyExc_Exception;
        tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_5, tmp_compexpr_right_5);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 51;
            type_description_1 = "ooooo";
            goto try_except_handler_4;
        }
        tmp_condition_result_7 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_7 == NUITKA_BOOL_TRUE) {
            goto branch_yes_7;
        } else {
            goto branch_no_7;
        }
        branch_yes_7:;
        {
            nuitka_bool tmp_condition_result_8;
            PyObject *tmp_source_name_13;
            PyObject *tmp_source_name_14;
            PyObject *tmp_mvar_value_12;
            PyObject *tmp_attribute_value_3;
            int tmp_truth_name_3;
            tmp_mvar_value_12 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_12 == NULL)) {
                tmp_mvar_value_12 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_12 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 52;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }

            tmp_source_name_14 = tmp_mvar_value_12;
            tmp_source_name_13 = LOOKUP_ATTRIBUTE(tmp_source_name_14, const_str_plain_flags);
            if (tmp_source_name_13 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 52;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_attribute_value_3 = LOOKUP_ATTRIBUTE(tmp_source_name_13, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_13);
            if (tmp_attribute_value_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 52;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_truth_name_3 = CHECK_IF_TRUE(tmp_attribute_value_3);
            if (tmp_truth_name_3 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_3);

                exception_lineno = 52;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_condition_result_8 = tmp_truth_name_3 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_3);
            if (tmp_condition_result_8 == NUITKA_BOOL_TRUE) {
                goto branch_yes_8;
            } else {
                goto branch_no_8;
            }
            branch_yes_8:;
            {
                PyObject *tmp_called_instance_6;
                PyObject *tmp_source_name_15;
                PyObject *tmp_mvar_value_13;
                PyObject *tmp_call_result_5;
                tmp_mvar_value_13 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_13 == NULL)) {
                    tmp_mvar_value_13 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_13 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 53;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }

                tmp_source_name_15 = tmp_mvar_value_13;
                tmp_called_instance_6 = LOOKUP_ATTRIBUTE(tmp_source_name_15, const_str_plain_stderr);
                if (tmp_called_instance_6 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 53;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 53;
                tmp_call_result_5 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_6, const_str_plain_write, &PyTuple_GET_ITEM(const_tuple_str_digest_db4a5a786e788512167378827f558f95_tuple, 0));

                Py_DECREF(tmp_called_instance_6);
                if (tmp_call_result_5 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 53;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                Py_DECREF(tmp_call_result_5);
            }
            {
                PyObject *tmp_called_instance_7;
                PyObject *tmp_source_name_16;
                PyObject *tmp_mvar_value_14;
                PyObject *tmp_call_result_6;
                tmp_mvar_value_14 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_14 == NULL)) {
                    tmp_mvar_value_14 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_14 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 54;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }

                tmp_source_name_16 = tmp_mvar_value_14;
                tmp_called_instance_7 = LOOKUP_ATTRIBUTE(tmp_source_name_16, const_str_plain_stderr);
                if (tmp_called_instance_7 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 54;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 54;
                tmp_call_result_6 = CALL_METHOD_NO_ARGS(tmp_called_instance_7, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_7);
                if (tmp_call_result_6 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 54;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                Py_DECREF(tmp_call_result_6);
            }
            branch_no_8:;
        }
        goto branch_end_7;
        branch_no_7:;
        tmp_result = RERAISE_EXCEPTION(&exception_type, &exception_value, &exception_tb);
        if (unlikely(tmp_result == false)) {
            exception_lineno = 48;
        }

        if (exception_tb && exception_tb->tb_frame == &frame_be1aab7b1485b4d13183f2905837edc4->m_frame) frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = exception_tb->tb_lineno;
        type_description_1 = "ooooo";
        goto try_except_handler_4;
        branch_end_7:;
    }
    goto try_end_3;
    // exception handler codes exits in all cases
    NUITKA_CANNOT_GET_HERE(runtime$$$function_1__putenv);
    return NULL;
    // End of try:
    try_end_3:;
    {
        nuitka_bool tmp_condition_result_9;
        nuitka_bool tmp_compexpr_left_6;
        nuitka_bool tmp_compexpr_right_6;
        assert(tmp_try_except_2__unhandled_indicator != NUITKA_BOOL_UNASSIGNED);
        tmp_compexpr_left_6 = tmp_try_except_2__unhandled_indicator;
        tmp_compexpr_right_6 = NUITKA_BOOL_TRUE;
        tmp_condition_result_9 = (tmp_compexpr_left_6 == tmp_compexpr_right_6) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_9 == NUITKA_BOOL_TRUE) {
            goto branch_yes_9;
        } else {
            goto branch_no_9;
        }
        branch_yes_9:;
        {
            nuitka_bool tmp_condition_result_10;
            PyObject *tmp_source_name_17;
            PyObject *tmp_source_name_18;
            PyObject *tmp_mvar_value_15;
            PyObject *tmp_attribute_value_4;
            int tmp_truth_name_4;
            tmp_mvar_value_15 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_15 == NULL)) {
                tmp_mvar_value_15 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_15 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 56;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }

            tmp_source_name_18 = tmp_mvar_value_15;
            tmp_source_name_17 = LOOKUP_ATTRIBUTE(tmp_source_name_18, const_str_plain_flags);
            if (tmp_source_name_17 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 56;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_attribute_value_4 = LOOKUP_ATTRIBUTE(tmp_source_name_17, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_17);
            if (tmp_attribute_value_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 56;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_truth_name_4 = CHECK_IF_TRUE(tmp_attribute_value_4);
            if (tmp_truth_name_4 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_4);

                exception_lineno = 56;
                type_description_1 = "ooooo";
                goto try_except_handler_4;
            }
            tmp_condition_result_10 = tmp_truth_name_4 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_4);
            if (tmp_condition_result_10 == NUITKA_BOOL_TRUE) {
                goto branch_yes_10;
            } else {
                goto branch_no_10;
            }
            branch_yes_10:;
            {
                PyObject *tmp_called_instance_8;
                PyObject *tmp_source_name_19;
                PyObject *tmp_mvar_value_16;
                PyObject *tmp_call_result_7;
                tmp_mvar_value_16 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_16 == NULL)) {
                    tmp_mvar_value_16 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_16 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 57;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }

                tmp_source_name_19 = tmp_mvar_value_16;
                tmp_called_instance_8 = LOOKUP_ATTRIBUTE(tmp_source_name_19, const_str_plain_stderr);
                if (tmp_called_instance_8 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 57;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 57;
                tmp_call_result_7 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_8, const_str_plain_write, &PyTuple_GET_ITEM(const_tuple_str_digest_70f1ee9ca166e607f11166ae647e026d_tuple, 0));

                Py_DECREF(tmp_called_instance_8);
                if (tmp_call_result_7 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 57;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                Py_DECREF(tmp_call_result_7);
            }
            {
                PyObject *tmp_called_instance_9;
                PyObject *tmp_source_name_20;
                PyObject *tmp_mvar_value_17;
                PyObject *tmp_call_result_8;
                tmp_mvar_value_17 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_17 == NULL)) {
                    tmp_mvar_value_17 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_17 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 58;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }

                tmp_source_name_20 = tmp_mvar_value_17;
                tmp_called_instance_9 = LOOKUP_ATTRIBUTE(tmp_source_name_20, const_str_plain_stderr);
                if (tmp_called_instance_9 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 58;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 58;
                tmp_call_result_8 = CALL_METHOD_NO_ARGS(tmp_called_instance_9, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_9);
                if (tmp_call_result_8 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 58;
                    type_description_1 = "ooooo";
                    goto try_except_handler_4;
                }
                Py_DECREF(tmp_call_result_8);
            }
            branch_no_10:;
        }
        branch_no_9:;
    }
    goto try_end_4;
    // Exception handler code:
    try_except_handler_4:;
    exception_keeper_type_4 = exception_type;
    exception_keeper_value_4 = exception_value;
    exception_keeper_tb_4 = exception_tb;
    exception_keeper_lineno_4 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    // Re-raise.
    exception_type = exception_keeper_type_4;
    exception_value = exception_keeper_value_4;
    exception_tb = exception_keeper_tb_4;
    exception_lineno = exception_keeper_lineno_4;

    goto frame_exception_exit_1;
    // End of try:
    try_end_4:;
    {
        nuitka_bool tmp_assign_source_7;
        tmp_assign_source_7 = NUITKA_BOOL_TRUE;
        tmp_try_except_3__unhandled_indicator = tmp_assign_source_7;
    }
    // Tried code:
    // Tried code:
    {
        PyObject *tmp_assign_source_8;
        PyObject *tmp_called_name_2;
        PyObject *tmp_mvar_value_18;
        tmp_mvar_value_18 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_find_msvcrt);

        if (unlikely(tmp_mvar_value_18 == NULL)) {
            tmp_mvar_value_18 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_find_msvcrt);
        }

        if (tmp_mvar_value_18 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 5311 ], 40, 0);
            exception_tb = NULL;

            exception_lineno = 62;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }

        tmp_called_name_2 = tmp_mvar_value_18;
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 62;
        tmp_assign_source_8 = CALL_FUNCTION_NO_ARGS(tmp_called_name_2);
        if (tmp_assign_source_8 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 62;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        assert(var_msvcrt == NULL);
        var_msvcrt = tmp_assign_source_8;
    }
    {
        PyObject *tmp_assign_source_9;
        nuitka_bool tmp_condition_result_11;
        PyObject *tmp_compexpr_left_7;
        PyObject *tmp_compexpr_right_7;
        PyObject *tmp_subscribed_name_1;
        PyObject *tmp_called_instance_10;
        PyObject *tmp_str_arg_1;
        PyObject *tmp_subscript_name_1;
        PyObject *tmp_str_arg_2;
        tmp_compexpr_left_7 = const_str_dot;
        CHECK_OBJECT(var_msvcrt);
        tmp_compexpr_right_7 = var_msvcrt;
        tmp_res = PySequence_Contains(tmp_compexpr_right_7, tmp_compexpr_left_7);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 63;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        tmp_condition_result_11 = (tmp_res == 1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_11 == NUITKA_BOOL_TRUE) {
            goto condexpr_true_1;
        } else {
            goto condexpr_false_1;
        }
        condexpr_true_1:;
        CHECK_OBJECT(var_msvcrt);
        tmp_str_arg_1 = var_msvcrt;
        tmp_called_instance_10 = PyObject_Str(tmp_str_arg_1);
        if (tmp_called_instance_10 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 63;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 63;
        tmp_subscribed_name_1 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_10, const_str_plain_split, &PyTuple_GET_ITEM(const_tuple_str_dot_tuple, 0));

        Py_DECREF(tmp_called_instance_10);
        if (tmp_subscribed_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 63;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        tmp_subscript_name_1 = const_int_0;
        tmp_assign_source_9 = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_1, tmp_subscript_name_1, 0);
        Py_DECREF(tmp_subscribed_name_1);
        if (tmp_assign_source_9 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 63;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        goto condexpr_end_1;
        condexpr_false_1:;
        CHECK_OBJECT(var_msvcrt);
        tmp_str_arg_2 = var_msvcrt;
        tmp_assign_source_9 = PyObject_Str(tmp_str_arg_2);
        if (tmp_assign_source_9 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 63;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        condexpr_end_1:;
        assert(var_msvcrtname == NULL);
        var_msvcrtname = tmp_assign_source_9;
    }
    {
        PyObject *tmp_assign_source_10;
        PyObject *tmp_called_name_3;
        PyObject *tmp_source_name_21;
        PyObject *tmp_called_instance_11;
        PyObject *tmp_mvar_value_19;
        PyObject *tmp_args_element_name_4;
        PyObject *tmp_args_element_name_5;
        PyObject *tmp_left_name_2;
        PyObject *tmp_right_name_2;
        PyObject *tmp_tuple_element_2;
        tmp_mvar_value_19 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_cdll);

        if (unlikely(tmp_mvar_value_19 == NULL)) {
            tmp_mvar_value_19 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_cdll);
        }

        if (tmp_mvar_value_19 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 5278 ], 33, 0);
            exception_tb = NULL;

            exception_lineno = 64;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }

        tmp_called_instance_11 = tmp_mvar_value_19;
        CHECK_OBJECT(var_msvcrt);
        tmp_args_element_name_4 = var_msvcrt;
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 64;
        {
            PyObject *call_args[] = {tmp_args_element_name_4};
            tmp_source_name_21 = CALL_METHOD_WITH_ARGS1(tmp_called_instance_11, const_str_plain_LoadLibrary, call_args);
        }

        if (tmp_source_name_21 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 64;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        tmp_called_name_3 = LOOKUP_ATTRIBUTE(tmp_source_name_21, const_str_plain__putenv);
        Py_DECREF(tmp_source_name_21);
        if (tmp_called_name_3 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 64;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        tmp_left_name_2 = const_str_digest_d26f52d432ccf199e53ad3ddfa46aa69;
        CHECK_OBJECT(par_name);
        tmp_tuple_element_2 = par_name;
        tmp_right_name_2 = PyTuple_New(2);
        Py_INCREF(tmp_tuple_element_2);
        PyTuple_SET_ITEM(tmp_right_name_2, 0, tmp_tuple_element_2);
        CHECK_OBJECT(par_value);
        tmp_tuple_element_2 = par_value;
        Py_INCREF(tmp_tuple_element_2);
        PyTuple_SET_ITEM(tmp_right_name_2, 1, tmp_tuple_element_2);
        tmp_args_element_name_5 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_2, tmp_right_name_2);
        Py_DECREF(tmp_right_name_2);
        if (tmp_args_element_name_5 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_called_name_3);

            exception_lineno = 64;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 64;
        tmp_assign_source_10 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_3, tmp_args_element_name_5);
        Py_DECREF(tmp_called_name_3);
        Py_DECREF(tmp_args_element_name_5);
        if (tmp_assign_source_10 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 64;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        {
            PyObject *old = var_result;
            var_result = tmp_assign_source_10;
            Py_XDECREF(old);
        }

    }
    {
        nuitka_bool tmp_condition_result_12;
        PyObject *tmp_compexpr_left_8;
        PyObject *tmp_compexpr_right_8;
        CHECK_OBJECT(var_result);
        tmp_compexpr_left_8 = var_result;
        tmp_compexpr_right_8 = const_int_0;
        tmp_res = RICH_COMPARE_BOOL_NOTEQ_OBJECT_INT(tmp_compexpr_left_8, tmp_compexpr_right_8);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 65;
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        tmp_condition_result_12 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_12 == NUITKA_BOOL_TRUE) {
            goto branch_yes_11;
        } else {
            goto branch_no_11;
        }
        branch_yes_11:;
        {
            PyObject *tmp_raise_type_3;
            PyObject *tmp_mvar_value_20;
            tmp_mvar_value_20 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_Warning);

            if (unlikely(tmp_mvar_value_20 == NULL)) {
                tmp_mvar_value_20 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_Warning);
            }

            if (tmp_mvar_value_20 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 5242 ], 36, 0);
                exception_tb = NULL;

                exception_lineno = 65;
                type_description_1 = "ooooo";
                goto try_except_handler_7;
            }

            tmp_raise_type_3 = tmp_mvar_value_20;
            exception_type = tmp_raise_type_3;
            Py_INCREF(tmp_raise_type_3);
            exception_lineno = 65;
            RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);
            type_description_1 = "ooooo";
            goto try_except_handler_7;
        }
        branch_no_11:;
    }
    goto try_end_5;
    // Exception handler code:
    try_except_handler_7:;
    exception_keeper_type_5 = exception_type;
    exception_keeper_value_5 = exception_value;
    exception_keeper_tb_5 = exception_tb;
    exception_keeper_lineno_5 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    {
        nuitka_bool tmp_assign_source_11;
        tmp_assign_source_11 = NUITKA_BOOL_FALSE;
        tmp_try_except_3__unhandled_indicator = tmp_assign_source_11;
    }
    // Preserve existing published exception.
    PRESERVE_FRAME_EXCEPTION(frame_be1aab7b1485b4d13183f2905837edc4);
    if (exception_keeper_tb_5 == NULL) {
        exception_keeper_tb_5 = MAKE_TRACEBACK(frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_5);
    } else if (exception_keeper_lineno_5 != 0) {
        exception_keeper_tb_5 = ADD_TRACEBACK(exception_keeper_tb_5, frame_be1aab7b1485b4d13183f2905837edc4, exception_keeper_lineno_5);
    }

    NORMALIZE_EXCEPTION(&exception_keeper_type_5, &exception_keeper_value_5, &exception_keeper_tb_5);
    PUBLISH_EXCEPTION(&exception_keeper_type_5, &exception_keeper_value_5, &exception_keeper_tb_5);
    {
        nuitka_bool tmp_condition_result_13;
        PyObject *tmp_compexpr_left_9;
        PyObject *tmp_compexpr_right_9;
        tmp_compexpr_left_9 = EXC_TYPE(PyThreadState_GET());
        tmp_compexpr_right_9 = PyExc_Exception;
        tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_9, tmp_compexpr_right_9);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 66;
            type_description_1 = "ooooo";
            goto try_except_handler_6;
        }
        tmp_condition_result_13 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_13 == NUITKA_BOOL_TRUE) {
            goto branch_yes_12;
        } else {
            goto branch_no_12;
        }
        branch_yes_12:;
        {
            nuitka_bool tmp_condition_result_14;
            PyObject *tmp_source_name_22;
            PyObject *tmp_source_name_23;
            PyObject *tmp_mvar_value_21;
            PyObject *tmp_attribute_value_5;
            int tmp_truth_name_5;
            tmp_mvar_value_21 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_21 == NULL)) {
                tmp_mvar_value_21 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_21 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 67;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }

            tmp_source_name_23 = tmp_mvar_value_21;
            tmp_source_name_22 = LOOKUP_ATTRIBUTE(tmp_source_name_23, const_str_plain_flags);
            if (tmp_source_name_22 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 67;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_attribute_value_5 = LOOKUP_ATTRIBUTE(tmp_source_name_22, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_22);
            if (tmp_attribute_value_5 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 67;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_truth_name_5 = CHECK_IF_TRUE(tmp_attribute_value_5);
            if (tmp_truth_name_5 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_5);

                exception_lineno = 67;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_condition_result_14 = tmp_truth_name_5 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_5);
            if (tmp_condition_result_14 == NUITKA_BOOL_TRUE) {
                goto branch_yes_13;
            } else {
                goto branch_no_13;
            }
            branch_yes_13:;
            {
                PyObject *tmp_called_name_4;
                PyObject *tmp_source_name_24;
                PyObject *tmp_source_name_25;
                PyObject *tmp_mvar_value_22;
                PyObject *tmp_call_result_9;
                PyObject *tmp_args_element_name_6;
                PyObject *tmp_left_name_3;
                PyObject *tmp_right_name_3;
                tmp_mvar_value_22 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_22 == NULL)) {
                    tmp_mvar_value_22 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_22 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_source_name_25 = tmp_mvar_value_22;
                tmp_source_name_24 = LOOKUP_ATTRIBUTE(tmp_source_name_25, const_str_plain_stderr);
                if (tmp_source_name_24 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                tmp_called_name_4 = LOOKUP_ATTRIBUTE(tmp_source_name_24, const_str_plain_write);
                Py_DECREF(tmp_source_name_24);
                if (tmp_called_name_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                tmp_left_name_3 = const_str_digest_25ea7f10d2ebb0c8b282b6c60bd61617;
                if (var_msvcrtname == NULL) {
                    Py_DECREF(tmp_called_name_4);
                    exception_type = PyExc_UnboundLocalError;
                    Py_INCREF(exception_type);
                    exception_value = PyString_FromFormat("local variable '%s' referenced before assignment", "msvcrtname");
                    exception_tb = NULL;

                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_right_name_3 = var_msvcrtname;
                tmp_args_element_name_6 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_3, tmp_right_name_3);
                if (tmp_args_element_name_6 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_called_name_4);

                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 68;
                tmp_call_result_9 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_4, tmp_args_element_name_6);
                Py_DECREF(tmp_called_name_4);
                Py_DECREF(tmp_args_element_name_6);
                if (tmp_call_result_9 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 68;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                Py_DECREF(tmp_call_result_9);
            }
            {
                PyObject *tmp_called_instance_12;
                PyObject *tmp_source_name_26;
                PyObject *tmp_mvar_value_23;
                PyObject *tmp_call_result_10;
                tmp_mvar_value_23 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_23 == NULL)) {
                    tmp_mvar_value_23 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_23 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 69;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_source_name_26 = tmp_mvar_value_23;
                tmp_called_instance_12 = LOOKUP_ATTRIBUTE(tmp_source_name_26, const_str_plain_stderr);
                if (tmp_called_instance_12 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 69;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 69;
                tmp_call_result_10 = CALL_METHOD_NO_ARGS(tmp_called_instance_12, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_12);
                if (tmp_call_result_10 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 69;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                Py_DECREF(tmp_call_result_10);
            }
            branch_no_13:;
        }
        goto branch_end_12;
        branch_no_12:;
        tmp_result = RERAISE_EXCEPTION(&exception_type, &exception_value, &exception_tb);
        if (unlikely(tmp_result == false)) {
            exception_lineno = 61;
        }

        if (exception_tb && exception_tb->tb_frame == &frame_be1aab7b1485b4d13183f2905837edc4->m_frame) frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = exception_tb->tb_lineno;
        type_description_1 = "ooooo";
        goto try_except_handler_6;
        branch_end_12:;
    }
    goto try_end_5;
    // exception handler codes exits in all cases
    NUITKA_CANNOT_GET_HERE(runtime$$$function_1__putenv);
    return NULL;
    // End of try:
    try_end_5:;
    {
        nuitka_bool tmp_condition_result_15;
        nuitka_bool tmp_compexpr_left_10;
        nuitka_bool tmp_compexpr_right_10;
        assert(tmp_try_except_3__unhandled_indicator != NUITKA_BOOL_UNASSIGNED);
        tmp_compexpr_left_10 = tmp_try_except_3__unhandled_indicator;
        tmp_compexpr_right_10 = NUITKA_BOOL_TRUE;
        tmp_condition_result_15 = (tmp_compexpr_left_10 == tmp_compexpr_right_10) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_15 == NUITKA_BOOL_TRUE) {
            goto branch_yes_14;
        } else {
            goto branch_no_14;
        }
        branch_yes_14:;
        {
            nuitka_bool tmp_condition_result_16;
            PyObject *tmp_source_name_27;
            PyObject *tmp_source_name_28;
            PyObject *tmp_mvar_value_24;
            PyObject *tmp_attribute_value_6;
            int tmp_truth_name_6;
            tmp_mvar_value_24 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

            if (unlikely(tmp_mvar_value_24 == NULL)) {
                tmp_mvar_value_24 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
            }

            if (tmp_mvar_value_24 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                exception_tb = NULL;

                exception_lineno = 71;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }

            tmp_source_name_28 = tmp_mvar_value_24;
            tmp_source_name_27 = LOOKUP_ATTRIBUTE(tmp_source_name_28, const_str_plain_flags);
            if (tmp_source_name_27 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 71;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_attribute_value_6 = LOOKUP_ATTRIBUTE(tmp_source_name_27, const_str_plain_verbose);
            Py_DECREF(tmp_source_name_27);
            if (tmp_attribute_value_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 71;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_truth_name_6 = CHECK_IF_TRUE(tmp_attribute_value_6);
            if (tmp_truth_name_6 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_attribute_value_6);

                exception_lineno = 71;
                type_description_1 = "ooooo";
                goto try_except_handler_6;
            }
            tmp_condition_result_16 = tmp_truth_name_6 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            Py_DECREF(tmp_attribute_value_6);
            if (tmp_condition_result_16 == NUITKA_BOOL_TRUE) {
                goto branch_yes_15;
            } else {
                goto branch_no_15;
            }
            branch_yes_15:;
            {
                PyObject *tmp_called_name_5;
                PyObject *tmp_source_name_29;
                PyObject *tmp_source_name_30;
                PyObject *tmp_mvar_value_25;
                PyObject *tmp_call_result_11;
                PyObject *tmp_args_element_name_7;
                PyObject *tmp_left_name_4;
                PyObject *tmp_right_name_4;
                tmp_mvar_value_25 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_25 == NULL)) {
                    tmp_mvar_value_25 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_25 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_source_name_30 = tmp_mvar_value_25;
                tmp_source_name_29 = LOOKUP_ATTRIBUTE(tmp_source_name_30, const_str_plain_stderr);
                if (tmp_source_name_29 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                tmp_called_name_5 = LOOKUP_ATTRIBUTE(tmp_source_name_29, const_str_plain_write);
                Py_DECREF(tmp_source_name_29);
                if (tmp_called_name_5 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                tmp_left_name_4 = const_str_digest_bc00a84b40c409ab8d40469dc39b0aac;
                if (var_msvcrtname == NULL) {
                    Py_DECREF(tmp_called_name_5);
                    exception_type = PyExc_UnboundLocalError;
                    Py_INCREF(exception_type);
                    exception_value = PyString_FromFormat("local variable '%s' referenced before assignment", "msvcrtname");
                    exception_tb = NULL;

                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_right_name_4 = var_msvcrtname;
                tmp_args_element_name_7 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_4, tmp_right_name_4);
                if (tmp_args_element_name_7 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_called_name_5);

                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 72;
                tmp_call_result_11 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_5, tmp_args_element_name_7);
                Py_DECREF(tmp_called_name_5);
                Py_DECREF(tmp_args_element_name_7);
                if (tmp_call_result_11 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 72;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                Py_DECREF(tmp_call_result_11);
            }
            {
                PyObject *tmp_called_instance_13;
                PyObject *tmp_source_name_31;
                PyObject *tmp_mvar_value_26;
                PyObject *tmp_call_result_12;
                tmp_mvar_value_26 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_26 == NULL)) {
                    tmp_mvar_value_26 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_26 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 204 ], 32, 0);
                    exception_tb = NULL;

                    exception_lineno = 73;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }

                tmp_source_name_31 = tmp_mvar_value_26;
                tmp_called_instance_13 = LOOKUP_ATTRIBUTE(tmp_source_name_31, const_str_plain_stderr);
                if (tmp_called_instance_13 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 73;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                frame_be1aab7b1485b4d13183f2905837edc4->m_frame.f_lineno = 73;
                tmp_call_result_12 = CALL_METHOD_NO_ARGS(tmp_called_instance_13, const_str_plain_flush);
                Py_DECREF(tmp_called_instance_13);
                if (tmp_call_result_12 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 73;
                    type_description_1 = "ooooo";
                    goto try_except_handler_6;
                }
                Py_DECREF(tmp_call_result_12);
            }
            branch_no_15:;
        }
        branch_no_14:;
    }
    goto try_end_6;
    // Exception handler code:
    try_except_handler_6:;
    exception_keeper_type_6 = exception_type;
    exception_keeper_value_6 = exception_value;
    exception_keeper_tb_6 = exception_tb;
    exception_keeper_lineno_6 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    // Re-raise.
    exception_type = exception_keeper_type_6;
    exception_value = exception_keeper_value_6;
    exception_tb = exception_keeper_tb_6;
    exception_lineno = exception_keeper_lineno_6;

    goto frame_exception_exit_1;
    // End of try:
    try_end_6:;

#if 1
    RESTORE_FRAME_EXCEPTION(frame_be1aab7b1485b4d13183f2905837edc4);
#endif

    // Put the previous frame back on top.
    popFrameStack();

    goto frame_no_exception_1;

    frame_exception_exit_1:;

#if 1
    RESTORE_FRAME_EXCEPTION(frame_be1aab7b1485b4d13183f2905837edc4);
#endif

    if (exception_tb == NULL) {
        exception_tb = MAKE_TRACEBACK(frame_be1aab7b1485b4d13183f2905837edc4, exception_lineno);
    }
    else if (exception_tb->tb_frame != &frame_be1aab7b1485b4d13183f2905837edc4->m_frame) {
        exception_tb = ADD_TRACEBACK(exception_tb, frame_be1aab7b1485b4d13183f2905837edc4, exception_lineno);
    }

    // Attachs locals to frame if any.
    Nuitka_Frame_AttachLocals(
        (struct Nuitka_FrameObject *)frame_be1aab7b1485b4d13183f2905837edc4,
        type_description_1,
        par_name,
        par_value,
        var_msvcrtname,
        var_msvcrt,
        var_result
    );


    // Release cached frame.
    if (frame_be1aab7b1485b4d13183f2905837edc4 == cache_frame_be1aab7b1485b4d13183f2905837edc4) {
        Py_DECREF(frame_be1aab7b1485b4d13183f2905837edc4);
    }
    cache_frame_be1aab7b1485b4d13183f2905837edc4 = NULL;

    assertFrameObject(frame_be1aab7b1485b4d13183f2905837edc4);

    // Put the previous frame back on top.
    popFrameStack();

    // Return the error.
    goto try_except_handler_1;

    frame_no_exception_1:;
    tmp_return_value = Py_None;
    Py_INCREF(tmp_return_value);
    goto try_return_handler_1;
    // tried codes exits in all cases
    NUITKA_CANNOT_GET_HERE(runtime$$$function_1__putenv);
    return NULL;
    // Return handler code:
    try_return_handler_1:;
    Py_XDECREF(var_msvcrtname);
    var_msvcrtname = NULL;

    Py_XDECREF(var_msvcrt);
    var_msvcrt = NULL;

    Py_XDECREF(var_result);
    var_result = NULL;

    goto function_return_exit;
    // Exception handler code:
    try_except_handler_1:;
    exception_keeper_type_7 = exception_type;
    exception_keeper_value_7 = exception_value;
    exception_keeper_tb_7 = exception_tb;
    exception_keeper_lineno_7 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(var_msvcrtname);
    var_msvcrtname = NULL;

    Py_XDECREF(var_msvcrt);
    var_msvcrt = NULL;

    Py_XDECREF(var_result);
    var_result = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_7;
    exception_value = exception_keeper_value_7;
    exception_tb = exception_keeper_tb_7;
    exception_lineno = exception_keeper_lineno_7;

    goto function_exception_exit;
    // End of try:

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(runtime$$$function_1__putenv);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_name);
    Py_DECREF(par_name);
    CHECK_OBJECT(par_value);
    Py_DECREF(par_value);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_name);
    Py_DECREF(par_name);
    CHECK_OBJECT(par_value);
    Py_DECREF(par_value);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}



static PyObject *MAKE_FUNCTION_runtime$$$function_1__putenv() {
    struct Nuitka_FunctionObject *result = Nuitka_Function_New(
        impl_runtime$$$function_1__putenv,
        const_str_plain__putenv,
#if PYTHON_VERSION >= 300
        NULL,
#endif
        codeobj_be1aab7b1485b4d13183f2905837edc4,
        NULL,
#if PYTHON_VERSION >= 300
        NULL,
        NULL,
#endif
        module_runtime,
        const_str_digest_85f5b38989fab5199ef6cd794aeca882,
        0
    );

    return (PyObject *)result;
}



#if PYTHON_VERSION >= 300
static struct PyModuleDef mdef_runtime =
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

function_impl_code functable_runtime[] = {
    impl_runtime$$$function_1__putenv,
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

    function_impl_code *current = functable_runtime;
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

    if (offset > sizeof(functable_runtime) || offset < 0) {
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
        functable_runtime[offset],
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
        module_runtime,
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
PyObject *modulecode_runtime(char const *module_full_name) {
#if defined(_NUITKA_EXE) || PYTHON_VERSION >= 300
    static bool _init_done = false;

    // Modules might be imported repeatedly, which is to be ignored.
    if (_init_done) {
        return module_runtime;
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
    PRINT_STRING("runtime: Calling setupMetaPathBasedLoader().\n");
#endif
    setupMetaPathBasedLoader();

#if PYTHON_VERSION >= 300
    patchInspectModule();
#endif

#endif

    /* The constants only used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("runtime: Calling createModuleConstants().\n");
#endif
    createModuleConstants();

    /* The code objects used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("runtime: Calling createModuleCodeObjects().\n");
#endif
    createModuleCodeObjects();

    // PRINT_STRING("in initruntime\n");

    // Create the module object first. There are no methods initially, all are
    // added dynamically in actual code only.  Also no "__doc__" is initially
    // set at this time, as it could not contain NUL characters this way, they
    // are instead set in early module code.  No "self" for modules, we have no
    // use for it.
#if PYTHON_VERSION < 300
    module_runtime = Py_InitModule4(
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
    mdef_runtime.m_name = module_full_name;
    module_runtime = PyModule_Create(&mdef_runtime);
#endif

    moduledict_runtime = MODULE_DICT(module_runtime);

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
        moduledict_runtime,
        (Nuitka_StringObject *)const_str_plain___compiled__,
        Nuitka_dunder_compiled_value
    );

    // Update "__package__" value to what it ought to be.
    {
#if 0
        UPDATE_STRING_DICT0(
            moduledict_runtime,
            (Nuitka_StringObject *)const_str_plain___package__,
            const_str_empty
        );
#elif 1
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___name__);

        UPDATE_STRING_DICT0(
            moduledict_runtime,
            (Nuitka_StringObject *)const_str_plain___package__,
            module_name
        );
#else

#if PYTHON_VERSION < 300
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___name__);
        char const *module_name_cstr = PyString_AS_STRING(module_name);

        char const *last_dot = strrchr(module_name_cstr, '.');

        if (last_dot != NULL)
        {
            UPDATE_STRING_DICT1(
                moduledict_runtime,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyString_FromStringAndSize(module_name_cstr, last_dot - module_name_cstr)
            );
        }
#else
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___name__);
        Py_ssize_t dot_index = PyUnicode_Find(module_name, const_str_dot, 0, PyUnicode_GetLength(module_name), -1);

        if (dot_index != -1)
        {
            UPDATE_STRING_DICT1(
                moduledict_runtime,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyUnicode_Substring(module_name, 0, dot_index)
            );
        }
#endif
#endif
    }

    CHECK_OBJECT(module_runtime);

// Seems to work for Python2.7 out of the box, but for Python3, the module
// doesn't automatically enter "sys.modules", so do it manually.
#if PYTHON_VERSION >= 300
    {
        int r = PyDict_SetItemString(PyImport_GetModuleDict(), module_full_name, module_runtime);
        assert(r != -1);
    }
#endif

    // For deep importing of a module we need to have "__builtins__", so we set
    // it ourselves in the same way than CPython does. Note: This must be done
    // before the frame object is allocated, or else it may fail.

    if (GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___builtins__) == NULL)
    {
        PyObject *value = (PyObject *)builtin_module;

        // Check if main module, not a dict then but the module itself.
#if !defined(_NUITKA_EXE) || !0
        value = PyModule_GetDict(value);
#endif

        UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___builtins__, value);
    }

#if PYTHON_VERSION >= 300
    UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___loader__, (PyObject *)&Nuitka_Loader_Type);
#endif

#if PYTHON_VERSION >= 340
// Set the "__spec__" value

#if 0
    // Main modules just get "None" as spec.
    UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___spec__, Py_None);
#else
    // Other modules get a "ModuleSpec" from the standard mechanism.
    {
        PyObject *bootstrap_module = PyImport_ImportModule("importlib._bootstrap");
        CHECK_OBJECT(bootstrap_module);
        PyObject *module_spec_class = PyObject_GetAttrString(bootstrap_module, "ModuleSpec");
        Py_DECREF(bootstrap_module);

        PyObject *args[] = {
            GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___name__),
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

        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___spec__, spec_value);
    }
#endif
#endif

    // Temp variables if any
    PyObject *tmp_list_contraction_1__$0 = NULL;
    PyObject *tmp_list_contraction_1__contraction_result = NULL;
    PyObject *tmp_list_contraction_1__iter_value_0 = NULL;
    struct Nuitka_FrameObject *frame_a80a886d96f093ab3a7b5df1f97de909;
    NUITKA_MAY_BE_UNUSED char const *type_description_1 = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    int tmp_res;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;

    // Module code.
    {
        PyObject *tmp_assign_source_1;
        tmp_assign_source_1 = Py_None;
        UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___doc__, tmp_assign_source_1);
    }
    {
        PyObject *tmp_assign_source_2;
        tmp_assign_source_2 = module_filename_obj;
        UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___file__, tmp_assign_source_2);
    }
    // Frame without reuse.
    frame_a80a886d96f093ab3a7b5df1f97de909 = MAKE_MODULE_FRAME(codeobj_a80a886d96f093ab3a7b5df1f97de909, module_runtime);

    // Push the new frame as the currently active one, and we should be exclusively
    // owning it.
    pushFrameStack(frame_a80a886d96f093ab3a7b5df1f97de909);
    assert(Py_REFCNT(frame_a80a886d96f093ab3a7b5df1f97de909) == 2);

    // Framed code:
    {
        PyObject *tmp_assign_source_3;
        PyObject *tmp_list_element_1;
        PyObject *tmp_called_name_1;
        PyObject *tmp_source_name_1;
        PyObject *tmp_args_element_name_1;
        PyObject *tmp_called_instance_1;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 1;
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
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 1;
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
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 1;
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
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 1;
        tmp_list_element_1 = CALL_METHOD_WITH_ARGS2(tmp_called_instance_1, const_str_plain_get, &PyTuple_GET_ITEM(const_tuple_3911f6818934bf315c20b2d1c4839f08_tuple, 0));

        if (tmp_list_element_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
            Py_DECREF(tmp_assign_source_3);

            exception_lineno = 1;

            goto frame_exception_exit_1;
        }
        PyList_SET_ITEM(tmp_assign_source_3, 1, tmp_list_element_1);
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___path__, tmp_assign_source_3);
    }
    {
        PyObject *tmp_assign_source_4;
        PyObject *tmp_name_name_1;
        PyObject *tmp_globals_name_1;
        PyObject *tmp_locals_name_1;
        PyObject *tmp_fromlist_name_1;
        tmp_name_name_1 = const_str_plain_os;
        tmp_globals_name_1 = (PyObject *)moduledict_runtime;
        tmp_locals_name_1 = Py_None;
        tmp_fromlist_name_1 = Py_None;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 4;
        tmp_assign_source_4 = IMPORT_MODULE4(tmp_name_name_1, tmp_globals_name_1, tmp_locals_name_1, tmp_fromlist_name_1);
        if (tmp_assign_source_4 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 4;

            goto frame_exception_exit_1;
        }
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os, tmp_assign_source_4);
    }
    {
        PyObject *tmp_assign_source_5;
        PyObject *tmp_name_name_2;
        PyObject *tmp_globals_name_2;
        PyObject *tmp_locals_name_2;
        PyObject *tmp_fromlist_name_2;
        tmp_name_name_2 = const_str_plain_sys;
        tmp_globals_name_2 = (PyObject *)moduledict_runtime;
        tmp_locals_name_2 = Py_None;
        tmp_fromlist_name_2 = Py_None;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 5;
        tmp_assign_source_5 = IMPORT_MODULE4(tmp_name_name_2, tmp_globals_name_2, tmp_locals_name_2, tmp_fromlist_name_2);
        assert(!(tmp_assign_source_5 == NULL));
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys, tmp_assign_source_5);
    }
    {
        PyObject *tmp_assign_source_6;
        PyObject *tmp_import_name_from_1;
        PyObject *tmp_name_name_3;
        PyObject *tmp_globals_name_3;
        PyObject *tmp_locals_name_3;
        PyObject *tmp_fromlist_name_3;
        tmp_name_name_3 = const_str_plain_ctypes;
        tmp_globals_name_3 = (PyObject *)moduledict_runtime;
        tmp_locals_name_3 = Py_None;
        tmp_fromlist_name_3 = const_tuple_str_plain_windll_tuple;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 7;
        tmp_import_name_from_1 = IMPORT_MODULE4(tmp_name_name_3, tmp_globals_name_3, tmp_locals_name_3, tmp_fromlist_name_3);
        if (tmp_import_name_from_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 7;

            goto frame_exception_exit_1;
        }
        tmp_assign_source_6 = IMPORT_NAME(tmp_import_name_from_1, const_str_plain_windll);
        Py_DECREF(tmp_import_name_from_1);
        if (tmp_assign_source_6 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 7;

            goto frame_exception_exit_1;
        }
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_windll, tmp_assign_source_6);
    }
    {
        PyObject *tmp_assign_source_7;
        PyObject *tmp_import_name_from_2;
        PyObject *tmp_name_name_4;
        PyObject *tmp_globals_name_4;
        PyObject *tmp_locals_name_4;
        PyObject *tmp_fromlist_name_4;
        tmp_name_name_4 = const_str_plain_ctypes;
        tmp_globals_name_4 = (PyObject *)moduledict_runtime;
        tmp_locals_name_4 = Py_None;
        tmp_fromlist_name_4 = const_tuple_str_plain_cdll_tuple;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 8;
        tmp_import_name_from_2 = IMPORT_MODULE4(tmp_name_name_4, tmp_globals_name_4, tmp_locals_name_4, tmp_fromlist_name_4);
        if (tmp_import_name_from_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 8;

            goto frame_exception_exit_1;
        }
        tmp_assign_source_7 = IMPORT_NAME(tmp_import_name_from_2, const_str_plain_cdll);
        Py_DECREF(tmp_import_name_from_2);
        if (tmp_assign_source_7 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 8;

            goto frame_exception_exit_1;
        }
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_cdll, tmp_assign_source_7);
    }
    {
        PyObject *tmp_assign_source_8;
        PyObject *tmp_import_name_from_3;
        PyObject *tmp_name_name_5;
        PyObject *tmp_globals_name_5;
        PyObject *tmp_locals_name_5;
        PyObject *tmp_fromlist_name_5;
        tmp_name_name_5 = const_str_digest_581e9157f4cfd7a80dd5ba063afd246e;
        tmp_globals_name_5 = (PyObject *)moduledict_runtime;
        tmp_locals_name_5 = Py_None;
        tmp_fromlist_name_5 = const_tuple_str_plain_find_msvcrt_tuple;
        frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 9;
        tmp_import_name_from_3 = IMPORT_MODULE4(tmp_name_name_5, tmp_globals_name_5, tmp_locals_name_5, tmp_fromlist_name_5);
        if (tmp_import_name_from_3 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 9;

            goto frame_exception_exit_1;
        }
        tmp_assign_source_8 = IMPORT_NAME(tmp_import_name_from_3, const_str_plain_find_msvcrt);
        Py_DECREF(tmp_import_name_from_3);
        if (tmp_assign_source_8 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 9;

            goto frame_exception_exit_1;
        }
        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_find_msvcrt, tmp_assign_source_8);
    }
    {
        PyObject *tmp_assign_source_9;
        tmp_assign_source_9 = MAKE_FUNCTION_runtime$$$function_1__putenv();



        UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain__putenv, tmp_assign_source_9);
    }
    {
        nuitka_bool tmp_condition_result_1;
        int tmp_or_left_truth_1;
        nuitka_bool tmp_or_left_value_1;
        nuitka_bool tmp_or_right_value_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_source_name_2;
        PyObject *tmp_mvar_value_1;
        PyObject *tmp_compexpr_left_2;
        PyObject *tmp_compexpr_right_2;
        PyObject *tmp_source_name_3;
        PyObject *tmp_mvar_value_2;
        tmp_mvar_value_1 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

        if (unlikely(tmp_mvar_value_1 == NULL)) {
            tmp_mvar_value_1 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
        }

        if (tmp_mvar_value_1 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
            exception_tb = NULL;

            exception_lineno = 76;

            goto frame_exception_exit_1;
        }

        tmp_source_name_2 = tmp_mvar_value_1;
        tmp_compexpr_left_1 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain_platform);
        if (tmp_compexpr_left_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 76;

            goto frame_exception_exit_1;
        }
        tmp_compexpr_right_1 = const_str_plain_win32;
        tmp_res = RICH_COMPARE_BOOL_EQ_OBJECT_OBJECT_NORECURSE(tmp_compexpr_left_1, tmp_compexpr_right_1);
        Py_DECREF(tmp_compexpr_left_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 76;

            goto frame_exception_exit_1;
        }
        tmp_or_left_value_1 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        tmp_or_left_truth_1 = tmp_or_left_value_1 == NUITKA_BOOL_TRUE ? 1 : 0;
        if (tmp_or_left_truth_1 == 1) {
            goto or_left_1;
        } else {
            goto or_right_1;
        }
        or_right_1:;
        tmp_mvar_value_2 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

        if (unlikely(tmp_mvar_value_2 == NULL)) {
            tmp_mvar_value_2 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
        }

        if (tmp_mvar_value_2 == NULL) {

            exception_type = PyExc_NameError;
            Py_INCREF(exception_type);
            exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
            exception_tb = NULL;

            exception_lineno = 76;

            goto frame_exception_exit_1;
        }

        tmp_source_name_3 = tmp_mvar_value_2;
        tmp_compexpr_left_2 = LOOKUP_ATTRIBUTE(tmp_source_name_3, const_str_plain_platform);
        if (tmp_compexpr_left_2 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 76;

            goto frame_exception_exit_1;
        }
        tmp_compexpr_right_2 = const_str_plain_nt;
        tmp_res = RICH_COMPARE_BOOL_EQ_OBJECT_OBJECT_NORECURSE(tmp_compexpr_left_2, tmp_compexpr_right_2);
        Py_DECREF(tmp_compexpr_left_2);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


            exception_lineno = 76;

            goto frame_exception_exit_1;
        }
        tmp_or_right_value_1 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        tmp_condition_result_1 = tmp_or_right_value_1;
        goto or_end_1;
        or_left_1:;
        tmp_condition_result_1 = tmp_or_left_value_1;
        or_end_1:;
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        {
            PyObject *tmp_assign_source_10;
            PyObject *tmp_called_name_2;
            PyObject *tmp_source_name_4;
            PyObject *tmp_source_name_5;
            PyObject *tmp_mvar_value_3;
            PyObject *tmp_args_element_name_2;
            PyObject *tmp_called_name_3;
            PyObject *tmp_source_name_6;
            PyObject *tmp_source_name_7;
            PyObject *tmp_mvar_value_4;
            PyObject *tmp_args_element_name_3;
            PyObject *tmp_called_name_4;
            PyObject *tmp_source_name_8;
            PyObject *tmp_source_name_9;
            PyObject *tmp_mvar_value_5;
            PyObject *tmp_args_element_name_4;
            PyObject *tmp_mvar_value_6;
            PyObject *tmp_args_element_name_5;
            tmp_mvar_value_3 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

            if (unlikely(tmp_mvar_value_3 == NULL)) {
                tmp_mvar_value_3 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
            }

            if (tmp_mvar_value_3 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                exception_tb = NULL;

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }

            tmp_source_name_5 = tmp_mvar_value_3;
            tmp_source_name_4 = LOOKUP_ATTRIBUTE(tmp_source_name_5, const_str_plain_path);
            if (tmp_source_name_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_called_name_2 = LOOKUP_ATTRIBUTE(tmp_source_name_4, const_str_plain_abspath);
            Py_DECREF(tmp_source_name_4);
            if (tmp_called_name_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_mvar_value_4 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

            if (unlikely(tmp_mvar_value_4 == NULL)) {
                tmp_mvar_value_4 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
            }

            if (tmp_mvar_value_4 == NULL) {
                Py_DECREF(tmp_called_name_2);
                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                exception_tb = NULL;

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }

            tmp_source_name_7 = tmp_mvar_value_4;
            tmp_source_name_6 = LOOKUP_ATTRIBUTE(tmp_source_name_7, const_str_plain_path);
            if (tmp_source_name_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_called_name_3 = LOOKUP_ATTRIBUTE(tmp_source_name_6, const_str_plain_join);
            Py_DECREF(tmp_source_name_6);
            if (tmp_called_name_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_mvar_value_5 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

            if (unlikely(tmp_mvar_value_5 == NULL)) {
                tmp_mvar_value_5 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
            }

            if (tmp_mvar_value_5 == NULL) {
                Py_DECREF(tmp_called_name_2);
                Py_DECREF(tmp_called_name_3);
                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                exception_tb = NULL;

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }

            tmp_source_name_9 = tmp_mvar_value_5;
            tmp_source_name_8 = LOOKUP_ATTRIBUTE(tmp_source_name_9, const_str_plain_path);
            if (tmp_source_name_8 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);
                Py_DECREF(tmp_called_name_3);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_called_name_4 = LOOKUP_ATTRIBUTE(tmp_source_name_8, const_str_plain_dirname);
            Py_DECREF(tmp_source_name_8);
            if (tmp_called_name_4 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);
                Py_DECREF(tmp_called_name_3);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_mvar_value_6 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain___file__);

            if (unlikely(tmp_mvar_value_6 == NULL)) {
                tmp_mvar_value_6 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain___file__);
            }

            if (tmp_mvar_value_6 == NULL) {
                Py_DECREF(tmp_called_name_2);
                Py_DECREF(tmp_called_name_3);
                Py_DECREF(tmp_called_name_4);
                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 5351 ], 30, 0);
                exception_tb = NULL;

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }

            tmp_args_element_name_4 = tmp_mvar_value_6;
            frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 77;
            tmp_args_element_name_3 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_4, tmp_args_element_name_4);
            Py_DECREF(tmp_called_name_4);
            if (tmp_args_element_name_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);
                Py_DECREF(tmp_called_name_3);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            tmp_args_element_name_5 = const_str_plain_bin;
            frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 77;
            {
                PyObject *call_args[] = {tmp_args_element_name_3, tmp_args_element_name_5};
                tmp_args_element_name_2 = CALL_FUNCTION_WITH_ARGS2(tmp_called_name_3, call_args);
            }

            Py_DECREF(tmp_called_name_3);
            Py_DECREF(tmp_args_element_name_3);
            if (tmp_args_element_name_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_2);

                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 77;
            tmp_assign_source_10 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_2, tmp_args_element_name_2);
            Py_DECREF(tmp_called_name_2);
            Py_DECREF(tmp_args_element_name_2);
            if (tmp_assign_source_10 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 77;

                goto frame_exception_exit_1;
            }
            UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_RUNTIME, tmp_assign_source_10);
        }
        {
            PyObject *tmp_assign_source_11;
            PyObject *tmp_called_name_5;
            PyObject *tmp_source_name_10;
            PyObject *tmp_subscribed_name_1;
            PyObject *tmp_source_name_11;
            PyObject *tmp_mvar_value_7;
            PyObject *tmp_subscript_name_1;
            PyObject *tmp_args_element_name_6;
            PyObject *tmp_source_name_12;
            PyObject *tmp_mvar_value_8;
            tmp_mvar_value_7 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

            if (unlikely(tmp_mvar_value_7 == NULL)) {
                tmp_mvar_value_7 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
            }

            if (tmp_mvar_value_7 == NULL) {

                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                exception_tb = NULL;

                exception_lineno = 78;

                goto frame_exception_exit_1;
            }

            tmp_source_name_11 = tmp_mvar_value_7;
            tmp_subscribed_name_1 = LOOKUP_ATTRIBUTE(tmp_source_name_11, const_str_plain_environ);
            if (tmp_subscribed_name_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 78;

                goto frame_exception_exit_1;
            }
            tmp_subscript_name_1 = const_str_plain_PATH;
            tmp_source_name_10 = LOOKUP_SUBSCRIPT(tmp_subscribed_name_1, tmp_subscript_name_1);
            Py_DECREF(tmp_subscribed_name_1);
            if (tmp_source_name_10 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 78;

                goto frame_exception_exit_1;
            }
            tmp_called_name_5 = LOOKUP_ATTRIBUTE(tmp_source_name_10, const_str_plain_split);
            Py_DECREF(tmp_source_name_10);
            if (tmp_called_name_5 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 78;

                goto frame_exception_exit_1;
            }
            tmp_mvar_value_8 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

            if (unlikely(tmp_mvar_value_8 == NULL)) {
                tmp_mvar_value_8 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
            }

            if (tmp_mvar_value_8 == NULL) {
                Py_DECREF(tmp_called_name_5);
                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                exception_tb = NULL;

                exception_lineno = 78;

                goto frame_exception_exit_1;
            }

            tmp_source_name_12 = tmp_mvar_value_8;
            tmp_args_element_name_6 = LOOKUP_ATTRIBUTE(tmp_source_name_12, const_str_plain_pathsep);
            if (tmp_args_element_name_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                Py_DECREF(tmp_called_name_5);

                exception_lineno = 78;

                goto frame_exception_exit_1;
            }
            frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 78;
            tmp_assign_source_11 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_5, tmp_args_element_name_6);
            Py_DECREF(tmp_called_name_5);
            Py_DECREF(tmp_args_element_name_6);
            if (tmp_assign_source_11 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 78;

                goto frame_exception_exit_1;
            }
            UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH, tmp_assign_source_11);
        }
        {
            PyObject *tmp_assign_source_12;
            // Tried code:
            {
                PyObject *tmp_assign_source_13;
                PyObject *tmp_iter_arg_1;
                PyObject *tmp_mvar_value_9;
                tmp_mvar_value_9 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH);

                if (unlikely(tmp_mvar_value_9 == NULL)) {
                    tmp_mvar_value_9 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_PATH);
                }

                CHECK_OBJECT(tmp_mvar_value_9);
                tmp_iter_arg_1 = tmp_mvar_value_9;
                tmp_assign_source_13 = MAKE_ITERATOR(tmp_iter_arg_1);
                if (tmp_assign_source_13 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 79;

                    goto try_except_handler_1;
                }
                assert(tmp_list_contraction_1__$0 == NULL);
                tmp_list_contraction_1__$0 = tmp_assign_source_13;
            }
            {
                PyObject *tmp_assign_source_14;
                tmp_assign_source_14 = PyList_New(0);
                assert(tmp_list_contraction_1__contraction_result == NULL);
                tmp_list_contraction_1__contraction_result = tmp_assign_source_14;
            }
            loop_start_1:;
            {
                PyObject *tmp_next_source_1;
                PyObject *tmp_assign_source_15;
                CHECK_OBJECT(tmp_list_contraction_1__$0);
                tmp_next_source_1 = tmp_list_contraction_1__$0;
                tmp_assign_source_15 = ITERATOR_NEXT(tmp_next_source_1);
                if (tmp_assign_source_15 == NULL) {
                    if (CHECK_AND_CLEAR_STOP_ITERATION_OCCURRED()) {

                        goto loop_end_1;
                    } else {

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                        exception_lineno = 79;
                        goto try_except_handler_1;
                    }
                }

                {
                    PyObject *old = tmp_list_contraction_1__iter_value_0;
                    tmp_list_contraction_1__iter_value_0 = tmp_assign_source_15;
                    Py_XDECREF(old);
                }

            }
            {
                PyObject *tmp_assign_source_16;
                CHECK_OBJECT(tmp_list_contraction_1__iter_value_0);
                tmp_assign_source_16 = tmp_list_contraction_1__iter_value_0;
                UPDATE_STRING_DICT0(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_x, tmp_assign_source_16);
            }
            {
                PyObject *tmp_append_list_1;
                PyObject *tmp_append_value_1;
                PyObject *tmp_called_name_6;
                PyObject *tmp_source_name_13;
                PyObject *tmp_source_name_14;
                PyObject *tmp_mvar_value_10;
                PyObject *tmp_args_element_name_7;
                PyObject *tmp_mvar_value_11;
                CHECK_OBJECT(tmp_list_contraction_1__contraction_result);
                tmp_append_list_1 = tmp_list_contraction_1__contraction_result;
                tmp_mvar_value_10 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

                if (unlikely(tmp_mvar_value_10 == NULL)) {
                    tmp_mvar_value_10 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
                }

                if (tmp_mvar_value_10 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                    exception_tb = NULL;

                    exception_lineno = 79;

                    goto try_except_handler_1;
                }

                tmp_source_name_14 = tmp_mvar_value_10;
                tmp_source_name_13 = LOOKUP_ATTRIBUTE(tmp_source_name_14, const_str_plain_path);
                if (tmp_source_name_13 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 79;

                    goto try_except_handler_1;
                }
                tmp_called_name_6 = LOOKUP_ATTRIBUTE(tmp_source_name_13, const_str_plain_abspath);
                Py_DECREF(tmp_source_name_13);
                if (tmp_called_name_6 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 79;

                    goto try_except_handler_1;
                }
                tmp_mvar_value_11 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_x);

                if (unlikely(tmp_mvar_value_11 == NULL)) {
                    tmp_mvar_value_11 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_x);
                }

                if (tmp_mvar_value_11 == NULL) {
                    Py_DECREF(tmp_called_name_6);
                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 5381 ], 23, 0);
                    exception_tb = NULL;

                    exception_lineno = 79;

                    goto try_except_handler_1;
                }

                tmp_args_element_name_7 = tmp_mvar_value_11;
                frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 79;
                tmp_append_value_1 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_6, tmp_args_element_name_7);
                Py_DECREF(tmp_called_name_6);
                if (tmp_append_value_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 79;

                    goto try_except_handler_1;
                }
                assert(PyList_Check(tmp_append_list_1));
                tmp_res = PyList_Append(tmp_append_list_1, tmp_append_value_1);
                Py_DECREF(tmp_append_value_1);
                if (tmp_res == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 79;

                    goto try_except_handler_1;
                }
            }
            if (CONSIDER_THREADING() == false) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 79;

                goto try_except_handler_1;
            }
            goto loop_start_1;
            loop_end_1:;
            CHECK_OBJECT(tmp_list_contraction_1__contraction_result);
            tmp_assign_source_12 = tmp_list_contraction_1__contraction_result;
            Py_INCREF(tmp_assign_source_12);
            goto try_return_handler_1;
            // tried codes exits in all cases
            NUITKA_CANNOT_GET_HERE(runtime);
            return NULL;
            // Return handler code:
            try_return_handler_1:;
            CHECK_OBJECT((PyObject *)tmp_list_contraction_1__$0);
            Py_DECREF(tmp_list_contraction_1__$0);
            tmp_list_contraction_1__$0 = NULL;

            CHECK_OBJECT((PyObject *)tmp_list_contraction_1__contraction_result);
            Py_DECREF(tmp_list_contraction_1__contraction_result);
            tmp_list_contraction_1__contraction_result = NULL;

            Py_XDECREF(tmp_list_contraction_1__iter_value_0);
            tmp_list_contraction_1__iter_value_0 = NULL;

            goto outline_result_1;
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

            Py_XDECREF(tmp_list_contraction_1__$0);
            tmp_list_contraction_1__$0 = NULL;

            Py_XDECREF(tmp_list_contraction_1__contraction_result);
            tmp_list_contraction_1__contraction_result = NULL;

            Py_XDECREF(tmp_list_contraction_1__iter_value_0);
            tmp_list_contraction_1__iter_value_0 = NULL;

            // Re-raise.
            exception_type = exception_keeper_type_1;
            exception_value = exception_keeper_value_1;
            exception_tb = exception_keeper_tb_1;
            exception_lineno = exception_keeper_lineno_1;

            goto frame_exception_exit_1;
            // End of try:
            // Return statement must have exited already.
            NUITKA_CANNOT_GET_HERE(runtime);
            return NULL;
            outline_result_1:;
            UPDATE_STRING_DICT1(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_ABSPATH, tmp_assign_source_12);
        }
        {
            nuitka_bool tmp_condition_result_2;
            PyObject *tmp_compexpr_left_3;
            PyObject *tmp_compexpr_right_3;
            PyObject *tmp_subscribed_name_2;
            PyObject *tmp_mvar_value_12;
            PyObject *tmp_subscript_name_2;
            PyObject *tmp_mvar_value_13;
            tmp_mvar_value_12 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_ABSPATH);

            if (unlikely(tmp_mvar_value_12 == NULL)) {
                tmp_mvar_value_12 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_ABSPATH);
            }

            CHECK_OBJECT(tmp_mvar_value_12);
            tmp_subscribed_name_2 = tmp_mvar_value_12;
            tmp_subscript_name_2 = const_int_0;
            tmp_compexpr_left_3 = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_2, tmp_subscript_name_2, 0);
            if (tmp_compexpr_left_3 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 81;

                goto frame_exception_exit_1;
            }
            tmp_mvar_value_13 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_RUNTIME);

            if (unlikely(tmp_mvar_value_13 == NULL)) {
                tmp_mvar_value_13 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_RUNTIME);
            }

            if (tmp_mvar_value_13 == NULL) {
                Py_DECREF(tmp_compexpr_left_3);
                exception_type = PyExc_NameError;
                Py_INCREF(exception_type);
                exception_value = UNSTREAM_STRING(&constant_bin[ 5404 ], 29, 0);
                exception_tb = NULL;

                exception_lineno = 81;

                goto frame_exception_exit_1;
            }

            tmp_compexpr_right_3 = tmp_mvar_value_13;
            tmp_res = RICH_COMPARE_BOOL_NOTEQ_OBJECT_OBJECT(tmp_compexpr_left_3, tmp_compexpr_right_3);
            Py_DECREF(tmp_compexpr_left_3);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                exception_lineno = 81;

                goto frame_exception_exit_1;
            }
            tmp_condition_result_2 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_2 == NUITKA_BOOL_TRUE) {
                goto branch_yes_2;
            } else {
                goto branch_no_2;
            }
            branch_yes_2:;
            {
                nuitka_bool tmp_condition_result_3;
                PyObject *tmp_source_name_15;
                PyObject *tmp_source_name_16;
                PyObject *tmp_mvar_value_14;
                PyObject *tmp_attribute_value_1;
                int tmp_truth_name_1;
                tmp_mvar_value_14 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_14 == NULL)) {
                    tmp_mvar_value_14 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_14 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                    exception_tb = NULL;

                    exception_lineno = 82;

                    goto frame_exception_exit_1;
                }

                tmp_source_name_16 = tmp_mvar_value_14;
                tmp_source_name_15 = LOOKUP_ATTRIBUTE(tmp_source_name_16, const_str_plain_flags);
                if (tmp_source_name_15 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 82;

                    goto frame_exception_exit_1;
                }
                tmp_attribute_value_1 = LOOKUP_ATTRIBUTE(tmp_source_name_15, const_str_plain_verbose);
                Py_DECREF(tmp_source_name_15);
                if (tmp_attribute_value_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 82;

                    goto frame_exception_exit_1;
                }
                tmp_truth_name_1 = CHECK_IF_TRUE(tmp_attribute_value_1);
                if (tmp_truth_name_1 == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_attribute_value_1);

                    exception_lineno = 82;

                    goto frame_exception_exit_1;
                }
                tmp_condition_result_3 = tmp_truth_name_1 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
                Py_DECREF(tmp_attribute_value_1);
                if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
                    goto branch_yes_3;
                } else {
                    goto branch_no_3;
                }
                branch_yes_3:;
                {
                    PyObject *tmp_called_name_7;
                    PyObject *tmp_source_name_17;
                    PyObject *tmp_source_name_18;
                    PyObject *tmp_mvar_value_15;
                    PyObject *tmp_call_result_1;
                    PyObject *tmp_args_element_name_8;
                    PyObject *tmp_left_name_1;
                    PyObject *tmp_right_name_1;
                    PyObject *tmp_mvar_value_16;
                    tmp_mvar_value_15 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_15 == NULL)) {
                        tmp_mvar_value_15 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_15 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_18 = tmp_mvar_value_15;
                    tmp_source_name_17 = LOOKUP_ATTRIBUTE(tmp_source_name_18, const_str_plain_stderr);
                    if (tmp_source_name_17 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_7 = LOOKUP_ATTRIBUTE(tmp_source_name_17, const_str_plain_write);
                    Py_DECREF(tmp_source_name_17);
                    if (tmp_called_name_7 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }
                    tmp_left_name_1 = const_str_digest_81daf1bf176566b0ca1361c8d199a6c0;
                    tmp_mvar_value_16 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_RUNTIME);

                    if (unlikely(tmp_mvar_value_16 == NULL)) {
                        tmp_mvar_value_16 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_RUNTIME);
                    }

                    if (tmp_mvar_value_16 == NULL) {
                        Py_DECREF(tmp_called_name_7);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 5404 ], 29, 0);
                        exception_tb = NULL;

                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }

                    tmp_right_name_1 = tmp_mvar_value_16;
                    tmp_args_element_name_8 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_1, tmp_right_name_1);
                    if (tmp_args_element_name_8 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_7);

                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 83;
                    tmp_call_result_1 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_7, tmp_args_element_name_8);
                    Py_DECREF(tmp_called_name_7);
                    Py_DECREF(tmp_args_element_name_8);
                    if (tmp_call_result_1 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 83;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_1);
                }
                {
                    PyObject *tmp_called_name_8;
                    PyObject *tmp_source_name_19;
                    PyObject *tmp_source_name_20;
                    PyObject *tmp_mvar_value_17;
                    PyObject *tmp_call_result_2;
                    PyObject *tmp_args_element_name_9;
                    PyObject *tmp_left_name_2;
                    PyObject *tmp_right_name_2;
                    PyObject *tmp_called_name_9;
                    PyObject *tmp_source_name_21;
                    PyObject *tmp_source_name_22;
                    PyObject *tmp_mvar_value_18;
                    PyObject *tmp_args_element_name_10;
                    PyObject *tmp_mvar_value_19;
                    tmp_mvar_value_17 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_17 == NULL)) {
                        tmp_mvar_value_17 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_17 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_20 = tmp_mvar_value_17;
                    tmp_source_name_19 = LOOKUP_ATTRIBUTE(tmp_source_name_20, const_str_plain_stderr);
                    if (tmp_source_name_19 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_8 = LOOKUP_ATTRIBUTE(tmp_source_name_19, const_str_plain_write);
                    Py_DECREF(tmp_source_name_19);
                    if (tmp_called_name_8 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    tmp_left_name_2 = const_str_digest_368a9f60f53f150afb585a023783bd94;
                    tmp_mvar_value_18 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

                    if (unlikely(tmp_mvar_value_18 == NULL)) {
                        tmp_mvar_value_18 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
                    }

                    if (tmp_mvar_value_18 == NULL) {
                        Py_DECREF(tmp_called_name_8);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                        exception_tb = NULL;

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_22 = tmp_mvar_value_18;
                    tmp_source_name_21 = LOOKUP_ATTRIBUTE(tmp_source_name_22, const_str_plain_pathsep);
                    if (tmp_source_name_21 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_8);

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_9 = LOOKUP_ATTRIBUTE(tmp_source_name_21, const_str_plain_join);
                    Py_DECREF(tmp_source_name_21);
                    if (tmp_called_name_9 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_8);

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    tmp_mvar_value_19 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH);

                    if (unlikely(tmp_mvar_value_19 == NULL)) {
                        tmp_mvar_value_19 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_PATH);
                    }

                    if (tmp_mvar_value_19 == NULL) {
                        Py_DECREF(tmp_called_name_8);
                        Py_DECREF(tmp_called_name_9);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 5433 ], 26, 0);
                        exception_tb = NULL;

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }

                    tmp_args_element_name_10 = tmp_mvar_value_19;
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 84;
                    tmp_right_name_2 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_9, tmp_args_element_name_10);
                    Py_DECREF(tmp_called_name_9);
                    if (tmp_right_name_2 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_8);

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    tmp_args_element_name_9 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_2, tmp_right_name_2);
                    Py_DECREF(tmp_right_name_2);
                    if (tmp_args_element_name_9 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_8);

                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 84;
                    tmp_call_result_2 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_8, tmp_args_element_name_9);
                    Py_DECREF(tmp_called_name_8);
                    Py_DECREF(tmp_args_element_name_9);
                    if (tmp_call_result_2 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 84;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_2);
                }
                {
                    PyObject *tmp_called_instance_2;
                    PyObject *tmp_source_name_23;
                    PyObject *tmp_mvar_value_20;
                    PyObject *tmp_call_result_3;
                    tmp_mvar_value_20 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_20 == NULL)) {
                        tmp_mvar_value_20 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_20 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 85;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_23 = tmp_mvar_value_20;
                    tmp_called_instance_2 = LOOKUP_ATTRIBUTE(tmp_source_name_23, const_str_plain_stderr);
                    if (tmp_called_instance_2 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 85;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 85;
                    tmp_call_result_3 = CALL_METHOD_NO_ARGS(tmp_called_instance_2, const_str_plain_flush);
                    Py_DECREF(tmp_called_instance_2);
                    if (tmp_call_result_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 85;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_3);
                }
                branch_no_3:;
            }
            {
                PyObject *tmp_called_name_10;
                PyObject *tmp_source_name_24;
                PyObject *tmp_mvar_value_21;
                PyObject *tmp_call_result_4;
                PyObject *tmp_args_element_name_11;
                PyObject *tmp_args_element_name_12;
                PyObject *tmp_mvar_value_22;
                tmp_mvar_value_21 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH);

                if (unlikely(tmp_mvar_value_21 == NULL)) {
                    tmp_mvar_value_21 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_PATH);
                }

                if (tmp_mvar_value_21 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 5433 ], 26, 0);
                    exception_tb = NULL;

                    exception_lineno = 87;

                    goto frame_exception_exit_1;
                }

                tmp_source_name_24 = tmp_mvar_value_21;
                tmp_called_name_10 = LOOKUP_ATTRIBUTE(tmp_source_name_24, const_str_plain_insert);
                if (tmp_called_name_10 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 87;

                    goto frame_exception_exit_1;
                }
                tmp_args_element_name_11 = const_int_0;
                tmp_mvar_value_22 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_RUNTIME);

                if (unlikely(tmp_mvar_value_22 == NULL)) {
                    tmp_mvar_value_22 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_RUNTIME);
                }

                if (tmp_mvar_value_22 == NULL) {
                    Py_DECREF(tmp_called_name_10);
                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 5404 ], 29, 0);
                    exception_tb = NULL;

                    exception_lineno = 87;

                    goto frame_exception_exit_1;
                }

                tmp_args_element_name_12 = tmp_mvar_value_22;
                frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 87;
                {
                    PyObject *call_args[] = {tmp_args_element_name_11, tmp_args_element_name_12};
                    tmp_call_result_4 = CALL_FUNCTION_WITH_ARGS2(tmp_called_name_10, call_args);
                }

                Py_DECREF(tmp_called_name_10);
                if (tmp_call_result_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 87;

                    goto frame_exception_exit_1;
                }
                Py_DECREF(tmp_call_result_4);
            }
            {
                PyObject *tmp_called_name_11;
                PyObject *tmp_mvar_value_23;
                PyObject *tmp_call_result_5;
                PyObject *tmp_args_element_name_13;
                PyObject *tmp_args_element_name_14;
                PyObject *tmp_called_name_12;
                PyObject *tmp_source_name_25;
                PyObject *tmp_source_name_26;
                PyObject *tmp_mvar_value_24;
                PyObject *tmp_args_element_name_15;
                PyObject *tmp_mvar_value_25;
                tmp_mvar_value_23 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain__putenv);

                if (unlikely(tmp_mvar_value_23 == NULL)) {
                    tmp_mvar_value_23 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain__putenv);
                }

                if (tmp_mvar_value_23 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 5459 ], 29, 0);
                    exception_tb = NULL;

                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }

                tmp_called_name_11 = tmp_mvar_value_23;
                tmp_args_element_name_13 = const_str_plain_PATH;
                tmp_mvar_value_24 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

                if (unlikely(tmp_mvar_value_24 == NULL)) {
                    tmp_mvar_value_24 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
                }

                if (tmp_mvar_value_24 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                    exception_tb = NULL;

                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }

                tmp_source_name_26 = tmp_mvar_value_24;
                tmp_source_name_25 = LOOKUP_ATTRIBUTE(tmp_source_name_26, const_str_plain_pathsep);
                if (tmp_source_name_25 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }
                tmp_called_name_12 = LOOKUP_ATTRIBUTE(tmp_source_name_25, const_str_plain_join);
                Py_DECREF(tmp_source_name_25);
                if (tmp_called_name_12 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }
                tmp_mvar_value_25 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH);

                if (unlikely(tmp_mvar_value_25 == NULL)) {
                    tmp_mvar_value_25 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_PATH);
                }

                if (tmp_mvar_value_25 == NULL) {
                    Py_DECREF(tmp_called_name_12);
                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 5433 ], 26, 0);
                    exception_tb = NULL;

                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }

                tmp_args_element_name_15 = tmp_mvar_value_25;
                frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 88;
                tmp_args_element_name_14 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_12, tmp_args_element_name_15);
                Py_DECREF(tmp_called_name_12);
                if (tmp_args_element_name_14 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }
                frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 88;
                {
                    PyObject *call_args[] = {tmp_args_element_name_13, tmp_args_element_name_14};
                    tmp_call_result_5 = CALL_FUNCTION_WITH_ARGS2(tmp_called_name_11, call_args);
                }

                Py_DECREF(tmp_args_element_name_14);
                if (tmp_call_result_5 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 88;

                    goto frame_exception_exit_1;
                }
                Py_DECREF(tmp_call_result_5);
            }
            {
                nuitka_bool tmp_condition_result_4;
                PyObject *tmp_source_name_27;
                PyObject *tmp_source_name_28;
                PyObject *tmp_mvar_value_26;
                PyObject *tmp_attribute_value_2;
                int tmp_truth_name_2;
                tmp_mvar_value_26 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_26 == NULL)) {
                    tmp_mvar_value_26 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_26 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                    exception_tb = NULL;

                    exception_lineno = 90;

                    goto frame_exception_exit_1;
                }

                tmp_source_name_28 = tmp_mvar_value_26;
                tmp_source_name_27 = LOOKUP_ATTRIBUTE(tmp_source_name_28, const_str_plain_flags);
                if (tmp_source_name_27 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 90;

                    goto frame_exception_exit_1;
                }
                tmp_attribute_value_2 = LOOKUP_ATTRIBUTE(tmp_source_name_27, const_str_plain_verbose);
                Py_DECREF(tmp_source_name_27);
                if (tmp_attribute_value_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 90;

                    goto frame_exception_exit_1;
                }
                tmp_truth_name_2 = CHECK_IF_TRUE(tmp_attribute_value_2);
                if (tmp_truth_name_2 == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_attribute_value_2);

                    exception_lineno = 90;

                    goto frame_exception_exit_1;
                }
                tmp_condition_result_4 = tmp_truth_name_2 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
                Py_DECREF(tmp_attribute_value_2);
                if (tmp_condition_result_4 == NUITKA_BOOL_TRUE) {
                    goto branch_yes_4;
                } else {
                    goto branch_no_4;
                }
                branch_yes_4:;
                {
                    PyObject *tmp_called_name_13;
                    PyObject *tmp_source_name_29;
                    PyObject *tmp_source_name_30;
                    PyObject *tmp_mvar_value_27;
                    PyObject *tmp_call_result_6;
                    PyObject *tmp_args_element_name_16;
                    PyObject *tmp_left_name_3;
                    PyObject *tmp_right_name_3;
                    PyObject *tmp_called_name_14;
                    PyObject *tmp_source_name_31;
                    PyObject *tmp_source_name_32;
                    PyObject *tmp_mvar_value_28;
                    PyObject *tmp_args_element_name_17;
                    PyObject *tmp_mvar_value_29;
                    tmp_mvar_value_27 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_27 == NULL)) {
                        tmp_mvar_value_27 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_27 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_30 = tmp_mvar_value_27;
                    tmp_source_name_29 = LOOKUP_ATTRIBUTE(tmp_source_name_30, const_str_plain_stderr);
                    if (tmp_source_name_29 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_13 = LOOKUP_ATTRIBUTE(tmp_source_name_29, const_str_plain_write);
                    Py_DECREF(tmp_source_name_29);
                    if (tmp_called_name_13 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    tmp_left_name_3 = const_str_digest_a2d9fabdc40336956ec28912a4fd7d01;
                    tmp_mvar_value_28 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_os);

                    if (unlikely(tmp_mvar_value_28 == NULL)) {
                        tmp_mvar_value_28 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_os);
                    }

                    if (tmp_mvar_value_28 == NULL) {
                        Py_DECREF(tmp_called_name_13);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 355 ], 24, 0);
                        exception_tb = NULL;

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_32 = tmp_mvar_value_28;
                    tmp_source_name_31 = LOOKUP_ATTRIBUTE(tmp_source_name_32, const_str_plain_pathsep);
                    if (tmp_source_name_31 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_13);

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_14 = LOOKUP_ATTRIBUTE(tmp_source_name_31, const_str_plain_join);
                    Py_DECREF(tmp_source_name_31);
                    if (tmp_called_name_14 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_13);

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    tmp_mvar_value_29 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_PATH);

                    if (unlikely(tmp_mvar_value_29 == NULL)) {
                        tmp_mvar_value_29 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_PATH);
                    }

                    if (tmp_mvar_value_29 == NULL) {
                        Py_DECREF(tmp_called_name_13);
                        Py_DECREF(tmp_called_name_14);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 5433 ], 26, 0);
                        exception_tb = NULL;

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }

                    tmp_args_element_name_17 = tmp_mvar_value_29;
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 91;
                    tmp_right_name_3 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_14, tmp_args_element_name_17);
                    Py_DECREF(tmp_called_name_14);
                    if (tmp_right_name_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_13);

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    tmp_args_element_name_16 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_3, tmp_right_name_3);
                    Py_DECREF(tmp_right_name_3);
                    if (tmp_args_element_name_16 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_13);

                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 91;
                    tmp_call_result_6 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_13, tmp_args_element_name_16);
                    Py_DECREF(tmp_called_name_13);
                    Py_DECREF(tmp_args_element_name_16);
                    if (tmp_call_result_6 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 91;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_6);
                }
                branch_no_4:;
            }
            goto branch_end_2;
            branch_no_2:;
            {
                nuitka_bool tmp_condition_result_5;
                PyObject *tmp_source_name_33;
                PyObject *tmp_source_name_34;
                PyObject *tmp_mvar_value_30;
                PyObject *tmp_attribute_value_3;
                int tmp_truth_name_3;
                tmp_mvar_value_30 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                if (unlikely(tmp_mvar_value_30 == NULL)) {
                    tmp_mvar_value_30 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                }

                if (tmp_mvar_value_30 == NULL) {

                    exception_type = PyExc_NameError;
                    Py_INCREF(exception_type);
                    exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                    exception_tb = NULL;

                    exception_lineno = 93;

                    goto frame_exception_exit_1;
                }

                tmp_source_name_34 = tmp_mvar_value_30;
                tmp_source_name_33 = LOOKUP_ATTRIBUTE(tmp_source_name_34, const_str_plain_flags);
                if (tmp_source_name_33 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 93;

                    goto frame_exception_exit_1;
                }
                tmp_attribute_value_3 = LOOKUP_ATTRIBUTE(tmp_source_name_33, const_str_plain_verbose);
                Py_DECREF(tmp_source_name_33);
                if (tmp_attribute_value_3 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    exception_lineno = 93;

                    goto frame_exception_exit_1;
                }
                tmp_truth_name_3 = CHECK_IF_TRUE(tmp_attribute_value_3);
                if (tmp_truth_name_3 == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_attribute_value_3);

                    exception_lineno = 93;

                    goto frame_exception_exit_1;
                }
                tmp_condition_result_5 = tmp_truth_name_3 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
                Py_DECREF(tmp_attribute_value_3);
                if (tmp_condition_result_5 == NUITKA_BOOL_TRUE) {
                    goto branch_yes_5;
                } else {
                    goto branch_no_5;
                }
                branch_yes_5:;
                {
                    PyObject *tmp_called_name_15;
                    PyObject *tmp_source_name_35;
                    PyObject *tmp_source_name_36;
                    PyObject *tmp_mvar_value_31;
                    PyObject *tmp_call_result_7;
                    PyObject *tmp_args_element_name_18;
                    PyObject *tmp_left_name_4;
                    PyObject *tmp_right_name_4;
                    PyObject *tmp_mvar_value_32;
                    tmp_mvar_value_31 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_31 == NULL)) {
                        tmp_mvar_value_31 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_31 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_36 = tmp_mvar_value_31;
                    tmp_source_name_35 = LOOKUP_ATTRIBUTE(tmp_source_name_36, const_str_plain_stderr);
                    if (tmp_source_name_35 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }
                    tmp_called_name_15 = LOOKUP_ATTRIBUTE(tmp_source_name_35, const_str_plain_write);
                    Py_DECREF(tmp_source_name_35);
                    if (tmp_called_name_15 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }
                    tmp_left_name_4 = const_str_digest_1e673c36590ff286a96617a71877a90e;
                    tmp_mvar_value_32 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_RUNTIME);

                    if (unlikely(tmp_mvar_value_32 == NULL)) {
                        tmp_mvar_value_32 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_RUNTIME);
                    }

                    if (tmp_mvar_value_32 == NULL) {
                        Py_DECREF(tmp_called_name_15);
                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 5404 ], 29, 0);
                        exception_tb = NULL;

                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }

                    tmp_right_name_4 = tmp_mvar_value_32;
                    tmp_args_element_name_18 = BINARY_OPERATION_MOD_STR_OBJECT(tmp_left_name_4, tmp_right_name_4);
                    if (tmp_args_element_name_18 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                        Py_DECREF(tmp_called_name_15);

                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 94;
                    tmp_call_result_7 = CALL_FUNCTION_WITH_SINGLE_ARG(tmp_called_name_15, tmp_args_element_name_18);
                    Py_DECREF(tmp_called_name_15);
                    Py_DECREF(tmp_args_element_name_18);
                    if (tmp_call_result_7 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 94;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_7);
                }
                {
                    PyObject *tmp_called_instance_3;
                    PyObject *tmp_source_name_37;
                    PyObject *tmp_mvar_value_33;
                    PyObject *tmp_call_result_8;
                    tmp_mvar_value_33 = GET_STRING_DICT_VALUE(moduledict_runtime, (Nuitka_StringObject *)const_str_plain_sys);

                    if (unlikely(tmp_mvar_value_33 == NULL)) {
                        tmp_mvar_value_33 = GET_STRING_DICT_VALUE(dict_builtin, (Nuitka_StringObject *)const_str_plain_sys);
                    }

                    if (tmp_mvar_value_33 == NULL) {

                        exception_type = PyExc_NameError;
                        Py_INCREF(exception_type);
                        exception_value = UNSTREAM_STRING(&constant_bin[ 211 ], 25, 0);
                        exception_tb = NULL;

                        exception_lineno = 95;

                        goto frame_exception_exit_1;
                    }

                    tmp_source_name_37 = tmp_mvar_value_33;
                    tmp_called_instance_3 = LOOKUP_ATTRIBUTE(tmp_source_name_37, const_str_plain_stderr);
                    if (tmp_called_instance_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 95;

                        goto frame_exception_exit_1;
                    }
                    frame_a80a886d96f093ab3a7b5df1f97de909->m_frame.f_lineno = 95;
                    tmp_call_result_8 = CALL_METHOD_NO_ARGS(tmp_called_instance_3, const_str_plain_flush);
                    Py_DECREF(tmp_called_instance_3);
                    if (tmp_call_result_8 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        exception_lineno = 95;

                        goto frame_exception_exit_1;
                    }
                    Py_DECREF(tmp_call_result_8);
                }
                branch_no_5:;
            }
            branch_end_2:;
        }
        branch_no_1:;
    }

    // Restore frame exception if necessary.
#if 0
    RESTORE_FRAME_EXCEPTION(frame_a80a886d96f093ab3a7b5df1f97de909);
#endif
    popFrameStack();

    assertFrameObject(frame_a80a886d96f093ab3a7b5df1f97de909);

    goto frame_no_exception_1;

    frame_exception_exit_1:;
#if 0
    RESTORE_FRAME_EXCEPTION(frame_a80a886d96f093ab3a7b5df1f97de909);
#endif

    if ( exception_tb == NULL )
    {
        exception_tb = MAKE_TRACEBACK(frame_a80a886d96f093ab3a7b5df1f97de909, exception_lineno);
    }
    else if (exception_tb->tb_frame != &frame_a80a886d96f093ab3a7b5df1f97de909->m_frame) {
        exception_tb = ADD_TRACEBACK(exception_tb, frame_a80a886d96f093ab3a7b5df1f97de909, exception_lineno);
    }

    // Put the previous frame back on top.
    popFrameStack();

    // Return the error.
    goto module_exception_exit;

    frame_no_exception_1:;

    return module_runtime;
    module_exception_exit:
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);
    return NULL;
}
