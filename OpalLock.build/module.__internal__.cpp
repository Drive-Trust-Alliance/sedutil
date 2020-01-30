/* Generated code for Python module '__internal__'
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

/* The "_module___internal__" is a Python object pointer of module type.
 *
 * Note: For full compatibility with CPython, every module variable access
 * needs to go through it except for cases where the module cannot possibly
 * have changed in the mean time.
 */

PyObject *module___internal__;
PyDictObject *moduledict___internal__;

/* The declarations of module constants used, if any. */
extern PyObject *const_str_plain_keys;
extern PyObject *const_str_digest_25731c733fd74e8333aa29126ce85686;
extern PyObject *const_int_0;
extern PyObject *const_tuple_348b0f87df4fe8a3ab054c1f070c95b2_tuple;
extern PyObject *const_str_plain___name__;
extern PyObject *const_str_digest_7bb187c562694df4ba2daaaf74308c83;
extern PyObject *const_str_digest_6fe42bc7767704653d0465c04f34143d;
extern PyObject *const_str_digest_5cf0452130fc4fd9d925cac8f015f9d1;
extern PyObject *const_str_plain_iteritems;
extern PyObject *const_str_plain_tuple;
extern PyObject *const_int_pos_1;
extern PyObject *const_str_digest_9816e8d1552296af90d250823c964059;
extern PyObject *const_str_plain_dict;
extern PyObject *const_str_digest_45e4dde2057b0bf276d6a84f4c917d27;
extern PyObject *const_str_digest_fdd1f239b4f3ffd6efcccf00e4a637e0;

/* Indicator if this modules private constants were created yet. */
static bool constants_created = false;

/* Function to create module private constants. */
static void createModuleConstants(void) {


    constants_created = true;
}

/* Function to verify module private constants for non-corruption. */
#ifndef __NUITKA_NO_ASSERT__
void checkModuleConstants___internal__(void) {
    // The module may not have been used at all, then ignore this.
    if (constants_created == false) return;


}
#endif

// The module code objects.


static void createModuleCodeObjects(void) {

}

// The module function declarations.
NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_1_complex_call_helper_star_list_star_dict(PyObject **python_pars);


NUITKA_LOCAL_MODULE PyObject *impl___internal__$$$function_2_get_callable_name_desc(PyObject **python_pars);


NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_3_complex_call_helper_pos_star_list_star_dict(PyObject **python_pars);


NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_4_complex_call_helper_pos_keywords_star_list_star_dict(PyObject **python_pars);


// The module function definitions.
NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_1_complex_call_helper_star_list_star_dict(PyObject **python_pars) {
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
    assert(!had_error); // Do not enter inlined functions with error set.
#endif

    // Local variable declarations.
    PyObject *par_called = python_pars[0];
    PyObject *par_star_arg_list = python_pars[1];
    PyObject *par_star_arg_dict = python_pars[2];
    PyObject *tmp_mapping_1__dict = NULL;
    PyObject *tmp_mapping_1__iter = NULL;
    PyObject *tmp_mapping_1__key = NULL;
    PyObject *tmp_mapping_1__keys = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;
    int tmp_res;
    PyObject *tmp_dictset_value;
    PyObject *tmp_dictset_dict;
    PyObject *tmp_dictset_key;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
    PyObject *exception_keeper_type_3;
    PyObject *exception_keeper_value_3;
    PyTracebackObject *exception_keeper_tb_3;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_3;
    PyObject *tmp_return_value = NULL;
    PyObject *exception_keeper_type_4;
    PyObject *exception_keeper_value_4;
    PyTracebackObject *exception_keeper_tb_4;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_4;

    // Actual function body.
    // Tried code:
    // Tried code:
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_type_arg_1;
        CHECK_OBJECT(par_star_arg_dict);
        tmp_type_arg_1 = par_star_arg_dict;
        tmp_compexpr_left_1 = BUILTIN_TYPE1(tmp_type_arg_1);
        assert(!(tmp_compexpr_left_1 == NULL));
        tmp_compexpr_right_1 = (PyObject *)&PyDict_Type;
        tmp_condition_result_1 = (tmp_compexpr_left_1 != tmp_compexpr_right_1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_1);
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        // Tried code:
        {
            PyObject *tmp_assign_source_1;
            PyObject *tmp_called_instance_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_called_instance_1 = par_star_arg_dict;
            tmp_assign_source_1 = CALL_METHOD_NO_ARGS(tmp_called_instance_1, const_str_plain_keys);
            if (tmp_assign_source_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_3;
            }
            assert(tmp_mapping_1__keys == NULL);
            tmp_mapping_1__keys = tmp_assign_source_1;
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
            nuitka_bool tmp_condition_result_2;
            PyObject *tmp_compexpr_left_2;
            PyObject *tmp_compexpr_right_2;
            tmp_compexpr_left_2 = exception_keeper_type_1;
            tmp_compexpr_right_2 = PyExc_AttributeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_2, tmp_compexpr_right_2);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_1);
                Py_XDECREF(exception_keeper_value_1);
                Py_XDECREF(exception_keeper_tb_1);



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
                PyObject *tmp_raise_type_1;
                PyObject *tmp_make_exception_arg_1;
                PyObject *tmp_left_name_1;
                PyObject *tmp_right_name_1;
                PyObject *tmp_tuple_element_1;
                PyObject *tmp_dircall_arg1_1;
                PyObject *tmp_source_name_1;
                PyObject *tmp_type_arg_2;
                tmp_left_name_1 = const_str_digest_9816e8d1552296af90d250823c964059;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_1 = par_called;
                Py_INCREF(tmp_dircall_arg1_1);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_1};
                    tmp_tuple_element_1 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_right_name_1 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_1, 0, tmp_tuple_element_1);
                CHECK_OBJECT(par_star_arg_dict);
                tmp_type_arg_2 = par_star_arg_dict;
                tmp_source_name_1 = BUILTIN_TYPE1(tmp_type_arg_2);
                assert(!(tmp_source_name_1 == NULL));
                tmp_tuple_element_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain___name__);
                Py_DECREF(tmp_source_name_1);
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_1);
                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                PyTuple_SET_ITEM(tmp_right_name_1, 1, tmp_tuple_element_1);
                tmp_make_exception_arg_1 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_1, tmp_right_name_1);
                Py_DECREF(tmp_right_name_1);
                if (tmp_make_exception_arg_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_raise_type_1 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_1);
                Py_DECREF(tmp_make_exception_arg_1);
                assert(!(tmp_raise_type_1 == NULL));
                exception_type = tmp_raise_type_1;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_2;
            }
            goto branch_end_2;
            branch_no_2:;
            // Re-raise.
            exception_type = exception_keeper_type_1;
            exception_value = exception_keeper_value_1;
            exception_tb = exception_keeper_tb_1;
            exception_lineno = exception_keeper_lineno_1;

            goto try_except_handler_2;
            branch_end_2:;
        }
        // End of try:
        try_end_1:;
        {
            PyObject *tmp_assign_source_2;
            PyObject *tmp_iter_arg_1;
            CHECK_OBJECT(tmp_mapping_1__keys);
            tmp_iter_arg_1 = tmp_mapping_1__keys;
            tmp_assign_source_2 = MAKE_ITERATOR(tmp_iter_arg_1);
            if (tmp_assign_source_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            assert(tmp_mapping_1__iter == NULL);
            tmp_mapping_1__iter = tmp_assign_source_2;
        }
        {
            PyObject *tmp_assign_source_3;
            tmp_assign_source_3 = PyDict_New();
            assert(tmp_mapping_1__dict == NULL);
            tmp_mapping_1__dict = tmp_assign_source_3;
        }
        loop_start_1:;
        {
            PyObject *tmp_next_source_1;
            PyObject *tmp_assign_source_4;
            CHECK_OBJECT(tmp_mapping_1__iter);
            tmp_next_source_1 = tmp_mapping_1__iter;
            tmp_assign_source_4 = ITERATOR_NEXT(tmp_next_source_1);
            if (tmp_assign_source_4 == NULL) {
                if (CHECK_AND_CLEAR_STOP_ITERATION_OCCURRED()) {

                    goto loop_end_1;
                } else {

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    goto try_except_handler_2;
                }
            }

            {
                PyObject *old = tmp_mapping_1__key;
                tmp_mapping_1__key = tmp_assign_source_4;
                Py_XDECREF(old);
            }

        }
        {
            PyObject *tmp_subscribed_name_1;
            PyObject *tmp_subscript_name_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_subscribed_name_1 = par_star_arg_dict;
            CHECK_OBJECT(tmp_mapping_1__key);
            tmp_subscript_name_1 = tmp_mapping_1__key;
            tmp_dictset_value = LOOKUP_SUBSCRIPT(tmp_subscribed_name_1, tmp_subscript_name_1);
            if (tmp_dictset_value == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            CHECK_OBJECT(tmp_mapping_1__dict);
            tmp_dictset_dict = tmp_mapping_1__dict;
            CHECK_OBJECT(tmp_mapping_1__key);
            tmp_dictset_key = tmp_mapping_1__key;
            tmp_res = PyDict_SetItem(tmp_dictset_dict, tmp_dictset_key, tmp_dictset_value);
            Py_DECREF(tmp_dictset_value);
            if (tmp_res != 0) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
        }
        if (CONSIDER_THREADING() == false) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_2;
        }
        goto loop_start_1;
        loop_end_1:;
        {
            PyObject *tmp_assign_source_5;
            CHECK_OBJECT(tmp_mapping_1__dict);
            tmp_assign_source_5 = tmp_mapping_1__dict;
            {
                PyObject *old = par_star_arg_dict;
                assert(old != NULL);
                par_star_arg_dict = tmp_assign_source_5;
                Py_INCREF(par_star_arg_dict);
                Py_DECREF(old);
            }

        }
        branch_no_1:;
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

    Py_XDECREF(tmp_mapping_1__dict);
    tmp_mapping_1__dict = NULL;

    Py_XDECREF(tmp_mapping_1__iter);
    tmp_mapping_1__iter = NULL;

    Py_XDECREF(tmp_mapping_1__keys);
    tmp_mapping_1__keys = NULL;

    Py_XDECREF(tmp_mapping_1__key);
    tmp_mapping_1__key = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_2;
    exception_value = exception_keeper_value_2;
    exception_tb = exception_keeper_tb_2;
    exception_lineno = exception_keeper_lineno_2;

    goto try_except_handler_1;
    // End of try:
    try_end_2:;
    Py_XDECREF(tmp_mapping_1__dict);
    tmp_mapping_1__dict = NULL;

    Py_XDECREF(tmp_mapping_1__iter);
    tmp_mapping_1__iter = NULL;

    Py_XDECREF(tmp_mapping_1__keys);
    tmp_mapping_1__keys = NULL;

    Py_XDECREF(tmp_mapping_1__key);
    tmp_mapping_1__key = NULL;

    {
        nuitka_bool tmp_condition_result_3;
        PyObject *tmp_compexpr_left_3;
        PyObject *tmp_compexpr_right_3;
        PyObject *tmp_type_arg_3;
        CHECK_OBJECT(par_star_arg_list);
        tmp_type_arg_3 = par_star_arg_list;
        tmp_compexpr_left_3 = BUILTIN_TYPE1(tmp_type_arg_3);
        assert(!(tmp_compexpr_left_3 == NULL));
        tmp_compexpr_right_3 = (PyObject *)&PyTuple_Type;
        tmp_condition_result_3 = (tmp_compexpr_left_3 != tmp_compexpr_right_3) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_3);
        if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
            goto branch_yes_3;
        } else {
            goto branch_no_3;
        }
        branch_yes_3:;
        // Tried code:
        {
            PyObject *tmp_assign_source_6;
            PyObject *tmp_tuple_arg_1;
            CHECK_OBJECT(par_star_arg_list);
            tmp_tuple_arg_1 = par_star_arg_list;
            tmp_assign_source_6 = PySequence_Tuple(tmp_tuple_arg_1);
            if (tmp_assign_source_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_4;
            }
            {
                PyObject *old = par_star_arg_list;
                assert(old != NULL);
                par_star_arg_list = tmp_assign_source_6;
                Py_DECREF(old);
            }

        }
        goto try_end_3;
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

        {
            nuitka_bool tmp_condition_result_4;
            PyObject *tmp_compexpr_left_4;
            PyObject *tmp_compexpr_right_4;
            tmp_compexpr_left_4 = exception_keeper_type_3;
            tmp_compexpr_right_4 = PyExc_TypeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_4, tmp_compexpr_right_4);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_3);
                Py_XDECREF(exception_keeper_value_3);
                Py_XDECREF(exception_keeper_tb_3);



                goto try_except_handler_1;
            }
            tmp_condition_result_4 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_4 == NUITKA_BOOL_TRUE) {
                goto branch_yes_4;
            } else {
                goto branch_no_4;
            }
            branch_yes_4:;
            {
                PyObject *tmp_raise_type_2;
                PyObject *tmp_make_exception_arg_2;
                PyObject *tmp_left_name_2;
                PyObject *tmp_right_name_2;
                PyObject *tmp_tuple_element_2;
                PyObject *tmp_dircall_arg1_2;
                PyObject *tmp_source_name_2;
                PyObject *tmp_type_arg_4;
                tmp_left_name_2 = const_str_digest_6fe42bc7767704653d0465c04f34143d;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_2 = par_called;
                Py_INCREF(tmp_dircall_arg1_2);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_2};
                    tmp_tuple_element_2 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_right_name_2 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_2, 0, tmp_tuple_element_2);
                CHECK_OBJECT(par_star_arg_list);
                tmp_type_arg_4 = par_star_arg_list;
                tmp_source_name_2 = BUILTIN_TYPE1(tmp_type_arg_4);
                assert(!(tmp_source_name_2 == NULL));
                tmp_tuple_element_2 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain___name__);
                Py_DECREF(tmp_source_name_2);
                if (tmp_tuple_element_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_2);
                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                PyTuple_SET_ITEM(tmp_right_name_2, 1, tmp_tuple_element_2);
                tmp_make_exception_arg_2 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_2, tmp_right_name_2);
                Py_DECREF(tmp_right_name_2);
                if (tmp_make_exception_arg_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_raise_type_2 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_2);
                Py_DECREF(tmp_make_exception_arg_2);
                assert(!(tmp_raise_type_2 == NULL));
                exception_type = tmp_raise_type_2;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_1;
            }
            goto branch_end_4;
            branch_no_4:;
            // Re-raise.
            exception_type = exception_keeper_type_3;
            exception_value = exception_keeper_value_3;
            exception_tb = exception_keeper_tb_3;
            exception_lineno = exception_keeper_lineno_3;

            goto try_except_handler_1;
            branch_end_4:;
        }
        // End of try:
        try_end_3:;
        branch_no_3:;
    }
    {
        PyObject *tmp_called_name_1;
        PyObject *tmp_args_name_1;
        PyObject *tmp_kw_name_1;
        CHECK_OBJECT(par_called);
        tmp_called_name_1 = par_called;
        CHECK_OBJECT(par_star_arg_list);
        tmp_args_name_1 = par_star_arg_list;
        CHECK_OBJECT(par_star_arg_dict);
        tmp_kw_name_1 = par_star_arg_dict;
        tmp_return_value = CALL_FUNCTION(tmp_called_name_1, tmp_args_name_1, tmp_kw_name_1);
        if (tmp_return_value == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_1;
        }
        goto try_return_handler_1;
    }
    // tried codes exits in all cases
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_1_complex_call_helper_star_list_star_dict);
    return NULL;
    // Return handler code:
    try_return_handler_1:;
    CHECK_OBJECT((PyObject *)par_star_arg_list);
    Py_DECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    CHECK_OBJECT((PyObject *)par_star_arg_dict);
    Py_DECREF(par_star_arg_dict);
    par_star_arg_dict = NULL;

    goto function_return_exit;
    // Exception handler code:
    try_except_handler_1:;
    exception_keeper_type_4 = exception_type;
    exception_keeper_value_4 = exception_value;
    exception_keeper_tb_4 = exception_tb;
    exception_keeper_lineno_4 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    Py_XDECREF(par_star_arg_dict);
    par_star_arg_dict = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_4;
    exception_value = exception_keeper_value_4;
    exception_tb = exception_keeper_tb_4;
    exception_lineno = exception_keeper_lineno_4;

    goto function_exception_exit;
    // End of try:

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_1_complex_call_helper_star_list_star_dict);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}


NUITKA_LOCAL_MODULE PyObject *impl___internal__$$$function_2_get_callable_name_desc(PyObject **python_pars) {
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
    assert(!had_error); // Do not enter inlined functions with error set.
#endif

    // Local variable declarations.
    PyObject *par_called = python_pars[0];
    int tmp_res;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    PyObject *tmp_return_value = NULL;

    // Actual function body.
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_isinstance_inst_1;
        PyObject *tmp_isinstance_cls_1;
        CHECK_OBJECT(par_called);
        tmp_isinstance_inst_1 = par_called;
        tmp_isinstance_cls_1 = const_tuple_348b0f87df4fe8a3ab054c1f070c95b2_tuple;
        tmp_res = Nuitka_IsInstance(tmp_isinstance_inst_1, tmp_isinstance_cls_1);
        if (tmp_res == -1) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto function_exception_exit;
        }
        tmp_condition_result_1 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        {
            PyObject *tmp_left_name_1;
            PyObject *tmp_source_name_1;
            PyObject *tmp_right_name_1;
            CHECK_OBJECT(par_called);
            tmp_source_name_1 = par_called;
            tmp_left_name_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain___name__);
            if (tmp_left_name_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto function_exception_exit;
            }
            tmp_right_name_1 = const_str_digest_25731c733fd74e8333aa29126ce85686;
            tmp_return_value = BINARY_OPERATION_ADD_OBJECT_STR(tmp_left_name_1, tmp_right_name_1);
            Py_DECREF(tmp_left_name_1);
            if (tmp_return_value == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto function_exception_exit;
            }
            goto function_return_exit;
        }
        goto branch_end_1;
        branch_no_1:;
        {
            nuitka_bool tmp_condition_result_2;
            PyObject *tmp_isinstance_inst_2;
            PyObject *tmp_isinstance_cls_2;
            CHECK_OBJECT(par_called);
            tmp_isinstance_inst_2 = par_called;
            tmp_isinstance_cls_2 = (PyObject *)&PyClass_Type;
            tmp_res = Nuitka_IsInstance(tmp_isinstance_inst_2, tmp_isinstance_cls_2);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto function_exception_exit;
            }
            tmp_condition_result_2 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_2 == NUITKA_BOOL_TRUE) {
                goto branch_yes_2;
            } else {
                goto branch_no_2;
            }
            branch_yes_2:;
            {
                PyObject *tmp_left_name_2;
                PyObject *tmp_source_name_2;
                PyObject *tmp_right_name_2;
                CHECK_OBJECT(par_called);
                tmp_source_name_2 = par_called;
                tmp_left_name_2 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain___name__);
                if (tmp_left_name_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto function_exception_exit;
                }
                tmp_right_name_2 = const_str_digest_5cf0452130fc4fd9d925cac8f015f9d1;
                tmp_return_value = BINARY_OPERATION_ADD_OBJECT_STR(tmp_left_name_2, tmp_right_name_2);
                Py_DECREF(tmp_left_name_2);
                if (tmp_return_value == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto function_exception_exit;
                }
                goto function_return_exit;
            }
            goto branch_end_2;
            branch_no_2:;
            {
                nuitka_bool tmp_condition_result_3;
                PyObject *tmp_isinstance_inst_3;
                PyObject *tmp_isinstance_cls_3;
                CHECK_OBJECT(par_called);
                tmp_isinstance_inst_3 = par_called;
                tmp_isinstance_cls_3 = (PyObject *)&PyInstance_Type;
                tmp_res = Nuitka_IsInstance(tmp_isinstance_inst_3, tmp_isinstance_cls_3);
                if (tmp_res == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto function_exception_exit;
                }
                tmp_condition_result_3 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
                if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
                    goto branch_yes_3;
                } else {
                    goto branch_no_3;
                }
                branch_yes_3:;
                {
                    PyObject *tmp_left_name_3;
                    PyObject *tmp_source_name_3;
                    PyObject *tmp_source_name_4;
                    PyObject *tmp_right_name_3;
                    CHECK_OBJECT(par_called);
                    tmp_source_name_4 = par_called;
                    tmp_source_name_3 = LOOKUP_ATTRIBUTE_CLASS_SLOT(tmp_source_name_4);
                    if (tmp_source_name_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto function_exception_exit;
                    }
                    tmp_left_name_3 = LOOKUP_ATTRIBUTE(tmp_source_name_3, const_str_plain___name__);
                    Py_DECREF(tmp_source_name_3);
                    if (tmp_left_name_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto function_exception_exit;
                    }
                    tmp_right_name_3 = const_str_digest_7bb187c562694df4ba2daaaf74308c83;
                    tmp_return_value = BINARY_OPERATION_ADD_OBJECT_STR(tmp_left_name_3, tmp_right_name_3);
                    Py_DECREF(tmp_left_name_3);
                    if (tmp_return_value == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto function_exception_exit;
                    }
                    goto function_return_exit;
                }
                goto branch_end_3;
                branch_no_3:;
                {
                    PyObject *tmp_left_name_4;
                    PyObject *tmp_source_name_5;
                    PyObject *tmp_type_arg_1;
                    PyObject *tmp_right_name_4;
                    CHECK_OBJECT(par_called);
                    tmp_type_arg_1 = par_called;
                    tmp_source_name_5 = BUILTIN_TYPE1(tmp_type_arg_1);
                    assert(!(tmp_source_name_5 == NULL));
                    tmp_left_name_4 = LOOKUP_ATTRIBUTE(tmp_source_name_5, const_str_plain___name__);
                    Py_DECREF(tmp_source_name_5);
                    if (tmp_left_name_4 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto function_exception_exit;
                    }
                    tmp_right_name_4 = const_str_digest_45e4dde2057b0bf276d6a84f4c917d27;
                    tmp_return_value = BINARY_OPERATION_ADD_OBJECT_STR(tmp_left_name_4, tmp_right_name_4);
                    Py_DECREF(tmp_left_name_4);
                    if (tmp_return_value == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto function_exception_exit;
                    }
                    goto function_return_exit;
                }
                branch_end_3:;
            }
            branch_end_2:;
        }
        branch_end_1:;
    }

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_2_get_callable_name_desc);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}


NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_3_complex_call_helper_pos_star_list_star_dict(PyObject **python_pars) {
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
    assert(!had_error); // Do not enter inlined functions with error set.
#endif

    // Local variable declarations.
    PyObject *par_called = python_pars[0];
    PyObject *par_args = python_pars[1];
    PyObject *par_star_arg_list = python_pars[2];
    PyObject *par_star_arg_dict = python_pars[3];
    PyObject *tmp_mapping_1__dict = NULL;
    PyObject *tmp_mapping_1__iter = NULL;
    PyObject *tmp_mapping_1__key = NULL;
    PyObject *tmp_mapping_1__keys = NULL;
    PyObject *exception_type = NULL;
    PyObject *exception_value = NULL;
    PyTracebackObject *exception_tb = NULL;
    NUITKA_MAY_BE_UNUSED int exception_lineno = 0;
    PyObject *exception_keeper_type_1;
    PyObject *exception_keeper_value_1;
    PyTracebackObject *exception_keeper_tb_1;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_1;
    int tmp_res;
    PyObject *tmp_dictset_value;
    PyObject *tmp_dictset_dict;
    PyObject *tmp_dictset_key;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
    PyObject *exception_keeper_type_3;
    PyObject *exception_keeper_value_3;
    PyTracebackObject *exception_keeper_tb_3;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_3;
    PyObject *tmp_return_value = NULL;
    PyObject *exception_keeper_type_4;
    PyObject *exception_keeper_value_4;
    PyTracebackObject *exception_keeper_tb_4;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_4;

    // Actual function body.
    // Tried code:
    // Tried code:
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_type_arg_1;
        CHECK_OBJECT(par_star_arg_dict);
        tmp_type_arg_1 = par_star_arg_dict;
        tmp_compexpr_left_1 = BUILTIN_TYPE1(tmp_type_arg_1);
        assert(!(tmp_compexpr_left_1 == NULL));
        tmp_compexpr_right_1 = (PyObject *)&PyDict_Type;
        tmp_condition_result_1 = (tmp_compexpr_left_1 != tmp_compexpr_right_1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_1);
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        // Tried code:
        {
            PyObject *tmp_assign_source_1;
            PyObject *tmp_called_instance_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_called_instance_1 = par_star_arg_dict;
            tmp_assign_source_1 = CALL_METHOD_NO_ARGS(tmp_called_instance_1, const_str_plain_keys);
            if (tmp_assign_source_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_3;
            }
            assert(tmp_mapping_1__keys == NULL);
            tmp_mapping_1__keys = tmp_assign_source_1;
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
            nuitka_bool tmp_condition_result_2;
            PyObject *tmp_compexpr_left_2;
            PyObject *tmp_compexpr_right_2;
            tmp_compexpr_left_2 = exception_keeper_type_1;
            tmp_compexpr_right_2 = PyExc_AttributeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_2, tmp_compexpr_right_2);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_1);
                Py_XDECREF(exception_keeper_value_1);
                Py_XDECREF(exception_keeper_tb_1);



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
                PyObject *tmp_raise_type_1;
                PyObject *tmp_make_exception_arg_1;
                PyObject *tmp_left_name_1;
                PyObject *tmp_right_name_1;
                PyObject *tmp_tuple_element_1;
                PyObject *tmp_dircall_arg1_1;
                PyObject *tmp_source_name_1;
                PyObject *tmp_type_arg_2;
                tmp_left_name_1 = const_str_digest_9816e8d1552296af90d250823c964059;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_1 = par_called;
                Py_INCREF(tmp_dircall_arg1_1);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_1};
                    tmp_tuple_element_1 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_right_name_1 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_1, 0, tmp_tuple_element_1);
                CHECK_OBJECT(par_star_arg_dict);
                tmp_type_arg_2 = par_star_arg_dict;
                tmp_source_name_1 = BUILTIN_TYPE1(tmp_type_arg_2);
                assert(!(tmp_source_name_1 == NULL));
                tmp_tuple_element_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain___name__);
                Py_DECREF(tmp_source_name_1);
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_1);
                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                PyTuple_SET_ITEM(tmp_right_name_1, 1, tmp_tuple_element_1);
                tmp_make_exception_arg_1 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_1, tmp_right_name_1);
                Py_DECREF(tmp_right_name_1);
                if (tmp_make_exception_arg_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_raise_type_1 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_1);
                Py_DECREF(tmp_make_exception_arg_1);
                assert(!(tmp_raise_type_1 == NULL));
                exception_type = tmp_raise_type_1;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_2;
            }
            goto branch_end_2;
            branch_no_2:;
            // Re-raise.
            exception_type = exception_keeper_type_1;
            exception_value = exception_keeper_value_1;
            exception_tb = exception_keeper_tb_1;
            exception_lineno = exception_keeper_lineno_1;

            goto try_except_handler_2;
            branch_end_2:;
        }
        // End of try:
        try_end_1:;
        {
            PyObject *tmp_assign_source_2;
            PyObject *tmp_iter_arg_1;
            CHECK_OBJECT(tmp_mapping_1__keys);
            tmp_iter_arg_1 = tmp_mapping_1__keys;
            tmp_assign_source_2 = MAKE_ITERATOR(tmp_iter_arg_1);
            if (tmp_assign_source_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            assert(tmp_mapping_1__iter == NULL);
            tmp_mapping_1__iter = tmp_assign_source_2;
        }
        {
            PyObject *tmp_assign_source_3;
            tmp_assign_source_3 = PyDict_New();
            assert(tmp_mapping_1__dict == NULL);
            tmp_mapping_1__dict = tmp_assign_source_3;
        }
        loop_start_1:;
        {
            PyObject *tmp_next_source_1;
            PyObject *tmp_assign_source_4;
            CHECK_OBJECT(tmp_mapping_1__iter);
            tmp_next_source_1 = tmp_mapping_1__iter;
            tmp_assign_source_4 = ITERATOR_NEXT(tmp_next_source_1);
            if (tmp_assign_source_4 == NULL) {
                if (CHECK_AND_CLEAR_STOP_ITERATION_OCCURRED()) {

                    goto loop_end_1;
                } else {

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    goto try_except_handler_2;
                }
            }

            {
                PyObject *old = tmp_mapping_1__key;
                tmp_mapping_1__key = tmp_assign_source_4;
                Py_XDECREF(old);
            }

        }
        {
            PyObject *tmp_subscribed_name_1;
            PyObject *tmp_subscript_name_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_subscribed_name_1 = par_star_arg_dict;
            CHECK_OBJECT(tmp_mapping_1__key);
            tmp_subscript_name_1 = tmp_mapping_1__key;
            tmp_dictset_value = LOOKUP_SUBSCRIPT(tmp_subscribed_name_1, tmp_subscript_name_1);
            if (tmp_dictset_value == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            CHECK_OBJECT(tmp_mapping_1__dict);
            tmp_dictset_dict = tmp_mapping_1__dict;
            CHECK_OBJECT(tmp_mapping_1__key);
            tmp_dictset_key = tmp_mapping_1__key;
            tmp_res = PyDict_SetItem(tmp_dictset_dict, tmp_dictset_key, tmp_dictset_value);
            Py_DECREF(tmp_dictset_value);
            if (tmp_res != 0) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
        }
        if (CONSIDER_THREADING() == false) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_2;
        }
        goto loop_start_1;
        loop_end_1:;
        {
            PyObject *tmp_assign_source_5;
            CHECK_OBJECT(tmp_mapping_1__dict);
            tmp_assign_source_5 = tmp_mapping_1__dict;
            {
                PyObject *old = par_star_arg_dict;
                assert(old != NULL);
                par_star_arg_dict = tmp_assign_source_5;
                Py_INCREF(par_star_arg_dict);
                Py_DECREF(old);
            }

        }
        branch_no_1:;
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

    Py_XDECREF(tmp_mapping_1__dict);
    tmp_mapping_1__dict = NULL;

    Py_XDECREF(tmp_mapping_1__iter);
    tmp_mapping_1__iter = NULL;

    Py_XDECREF(tmp_mapping_1__keys);
    tmp_mapping_1__keys = NULL;

    Py_XDECREF(tmp_mapping_1__key);
    tmp_mapping_1__key = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_2;
    exception_value = exception_keeper_value_2;
    exception_tb = exception_keeper_tb_2;
    exception_lineno = exception_keeper_lineno_2;

    goto try_except_handler_1;
    // End of try:
    try_end_2:;
    Py_XDECREF(tmp_mapping_1__dict);
    tmp_mapping_1__dict = NULL;

    Py_XDECREF(tmp_mapping_1__iter);
    tmp_mapping_1__iter = NULL;

    Py_XDECREF(tmp_mapping_1__keys);
    tmp_mapping_1__keys = NULL;

    Py_XDECREF(tmp_mapping_1__key);
    tmp_mapping_1__key = NULL;

    {
        nuitka_bool tmp_condition_result_3;
        PyObject *tmp_compexpr_left_3;
        PyObject *tmp_compexpr_right_3;
        PyObject *tmp_type_arg_3;
        CHECK_OBJECT(par_star_arg_list);
        tmp_type_arg_3 = par_star_arg_list;
        tmp_compexpr_left_3 = BUILTIN_TYPE1(tmp_type_arg_3);
        assert(!(tmp_compexpr_left_3 == NULL));
        tmp_compexpr_right_3 = (PyObject *)&PyTuple_Type;
        tmp_condition_result_3 = (tmp_compexpr_left_3 != tmp_compexpr_right_3) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_3);
        if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
            goto branch_yes_3;
        } else {
            goto branch_no_3;
        }
        branch_yes_3:;
        // Tried code:
        {
            PyObject *tmp_assign_source_6;
            PyObject *tmp_tuple_arg_1;
            CHECK_OBJECT(par_star_arg_list);
            tmp_tuple_arg_1 = par_star_arg_list;
            tmp_assign_source_6 = PySequence_Tuple(tmp_tuple_arg_1);
            if (tmp_assign_source_6 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_4;
            }
            {
                PyObject *old = par_star_arg_list;
                assert(old != NULL);
                par_star_arg_list = tmp_assign_source_6;
                Py_DECREF(old);
            }

        }
        goto try_end_3;
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

        {
            nuitka_bool tmp_condition_result_4;
            PyObject *tmp_compexpr_left_4;
            PyObject *tmp_compexpr_right_4;
            tmp_compexpr_left_4 = exception_keeper_type_3;
            tmp_compexpr_right_4 = PyExc_TypeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_4, tmp_compexpr_right_4);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_3);
                Py_XDECREF(exception_keeper_value_3);
                Py_XDECREF(exception_keeper_tb_3);



                goto try_except_handler_1;
            }
            tmp_condition_result_4 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_4 == NUITKA_BOOL_TRUE) {
                goto branch_yes_4;
            } else {
                goto branch_no_4;
            }
            branch_yes_4:;
            {
                PyObject *tmp_raise_type_2;
                PyObject *tmp_make_exception_arg_2;
                PyObject *tmp_left_name_2;
                PyObject *tmp_right_name_2;
                PyObject *tmp_tuple_element_2;
                PyObject *tmp_dircall_arg1_2;
                PyObject *tmp_source_name_2;
                PyObject *tmp_type_arg_4;
                tmp_left_name_2 = const_str_digest_6fe42bc7767704653d0465c04f34143d;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_2 = par_called;
                Py_INCREF(tmp_dircall_arg1_2);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_2};
                    tmp_tuple_element_2 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_right_name_2 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_2, 0, tmp_tuple_element_2);
                CHECK_OBJECT(par_star_arg_list);
                tmp_type_arg_4 = par_star_arg_list;
                tmp_source_name_2 = BUILTIN_TYPE1(tmp_type_arg_4);
                assert(!(tmp_source_name_2 == NULL));
                tmp_tuple_element_2 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain___name__);
                Py_DECREF(tmp_source_name_2);
                if (tmp_tuple_element_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_2);
                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                PyTuple_SET_ITEM(tmp_right_name_2, 1, tmp_tuple_element_2);
                tmp_make_exception_arg_2 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_2, tmp_right_name_2);
                Py_DECREF(tmp_right_name_2);
                if (tmp_make_exception_arg_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_raise_type_2 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_2);
                Py_DECREF(tmp_make_exception_arg_2);
                assert(!(tmp_raise_type_2 == NULL));
                exception_type = tmp_raise_type_2;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_1;
            }
            goto branch_end_4;
            branch_no_4:;
            // Re-raise.
            exception_type = exception_keeper_type_3;
            exception_value = exception_keeper_value_3;
            exception_tb = exception_keeper_tb_3;
            exception_lineno = exception_keeper_lineno_3;

            goto try_except_handler_1;
            branch_end_4:;
        }
        // End of try:
        try_end_3:;
        branch_no_3:;
    }
    {
        PyObject *tmp_called_name_1;
        PyObject *tmp_args_name_1;
        PyObject *tmp_left_name_3;
        PyObject *tmp_right_name_3;
        PyObject *tmp_kw_name_1;
        CHECK_OBJECT(par_called);
        tmp_called_name_1 = par_called;
        CHECK_OBJECT(par_args);
        tmp_left_name_3 = par_args;
        CHECK_OBJECT(par_star_arg_list);
        tmp_right_name_3 = par_star_arg_list;
        tmp_args_name_1 = BINARY_OPERATION_ADD_OBJECT_OBJECT(tmp_left_name_3, tmp_right_name_3);
        if (tmp_args_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_1;
        }
        CHECK_OBJECT(par_star_arg_dict);
        tmp_kw_name_1 = par_star_arg_dict;
        tmp_return_value = CALL_FUNCTION(tmp_called_name_1, tmp_args_name_1, tmp_kw_name_1);
        Py_DECREF(tmp_args_name_1);
        if (tmp_return_value == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_1;
        }
        goto try_return_handler_1;
    }
    // tried codes exits in all cases
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_3_complex_call_helper_pos_star_list_star_dict);
    return NULL;
    // Return handler code:
    try_return_handler_1:;
    CHECK_OBJECT((PyObject *)par_star_arg_list);
    Py_DECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    CHECK_OBJECT((PyObject *)par_star_arg_dict);
    Py_DECREF(par_star_arg_dict);
    par_star_arg_dict = NULL;

    goto function_return_exit;
    // Exception handler code:
    try_except_handler_1:;
    exception_keeper_type_4 = exception_type;
    exception_keeper_value_4 = exception_value;
    exception_keeper_tb_4 = exception_tb;
    exception_keeper_lineno_4 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    Py_XDECREF(par_star_arg_dict);
    par_star_arg_dict = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_4;
    exception_value = exception_keeper_value_4;
    exception_tb = exception_keeper_tb_4;
    exception_lineno = exception_keeper_lineno_4;

    goto function_exception_exit;
    // End of try:

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_3_complex_call_helper_pos_star_list_star_dict);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_args);
    Py_DECREF(par_args);
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_args);
    Py_DECREF(par_args);
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}


NUITKA_CROSS_MODULE PyObject *impl___internal__$$$function_4_complex_call_helper_pos_keywords_star_list_star_dict(PyObject **python_pars) {
#ifndef __NUITKA_NO_ASSERT__
    NUITKA_MAY_BE_UNUSED bool had_error = ERROR_OCCURRED();
    assert(!had_error); // Do not enter inlined functions with error set.
#endif

    // Local variable declarations.
    PyObject *par_called = python_pars[0];
    PyObject *par_args = python_pars[1];
    PyObject *par_kw = python_pars[2];
    PyObject *par_star_arg_list = python_pars[3];
    PyObject *par_star_arg_dict = python_pars[4];
    PyObject *tmp_dict_1__iter = NULL;
    PyObject *tmp_dict_1__key_xxx = NULL;
    PyObject *tmp_dict_1__keys = NULL;
    PyObject *tmp_dict_2__item = NULL;
    PyObject *tmp_dict_2__iter = NULL;
    PyObject *tmp_dict_2__key = NULL;
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
    PyObject *tmp_dictset_value;
    PyObject *tmp_dictset_dict;
    PyObject *tmp_dictset_key;
    PyObject *exception_keeper_type_2;
    PyObject *exception_keeper_value_2;
    PyTracebackObject *exception_keeper_tb_2;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_2;
    PyObject *exception_keeper_type_3;
    PyObject *exception_keeper_value_3;
    PyTracebackObject *exception_keeper_tb_3;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_3;
    PyObject *tmp_return_value = NULL;
    PyObject *exception_keeper_type_4;
    PyObject *exception_keeper_value_4;
    PyTracebackObject *exception_keeper_tb_4;
    NUITKA_MAY_BE_UNUSED int exception_keeper_lineno_4;

    // Actual function body.
    // Tried code:
    // Tried code:
    {
        nuitka_bool tmp_condition_result_1;
        PyObject *tmp_compexpr_left_1;
        PyObject *tmp_compexpr_right_1;
        PyObject *tmp_type_arg_1;
        CHECK_OBJECT(par_star_arg_dict);
        tmp_type_arg_1 = par_star_arg_dict;
        tmp_compexpr_left_1 = BUILTIN_TYPE1(tmp_type_arg_1);
        assert(!(tmp_compexpr_left_1 == NULL));
        tmp_compexpr_right_1 = (PyObject *)&PyDict_Type;
        tmp_condition_result_1 = (tmp_compexpr_left_1 != tmp_compexpr_right_1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_1);
        if (tmp_condition_result_1 == NUITKA_BOOL_TRUE) {
            goto branch_yes_1;
        } else {
            goto branch_no_1;
        }
        branch_yes_1:;
        // Tried code:
        {
            PyObject *tmp_assign_source_1;
            PyObject *tmp_called_instance_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_called_instance_1 = par_star_arg_dict;
            tmp_assign_source_1 = CALL_METHOD_NO_ARGS(tmp_called_instance_1, const_str_plain_keys);
            if (tmp_assign_source_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_3;
            }
            assert(tmp_dict_1__keys == NULL);
            tmp_dict_1__keys = tmp_assign_source_1;
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
            nuitka_bool tmp_condition_result_2;
            PyObject *tmp_compexpr_left_2;
            PyObject *tmp_compexpr_right_2;
            tmp_compexpr_left_2 = exception_keeper_type_1;
            tmp_compexpr_right_2 = PyExc_AttributeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_2, tmp_compexpr_right_2);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_1);
                Py_XDECREF(exception_keeper_value_1);
                Py_XDECREF(exception_keeper_tb_1);



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
                PyObject *tmp_raise_type_1;
                PyObject *tmp_make_exception_arg_1;
                PyObject *tmp_left_name_1;
                PyObject *tmp_right_name_1;
                PyObject *tmp_tuple_element_1;
                PyObject *tmp_dircall_arg1_1;
                PyObject *tmp_source_name_1;
                PyObject *tmp_type_arg_2;
                tmp_left_name_1 = const_str_digest_9816e8d1552296af90d250823c964059;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_1 = par_called;
                Py_INCREF(tmp_dircall_arg1_1);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_1};
                    tmp_tuple_element_1 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_right_name_1 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_1, 0, tmp_tuple_element_1);
                CHECK_OBJECT(par_star_arg_dict);
                tmp_type_arg_2 = par_star_arg_dict;
                tmp_source_name_1 = BUILTIN_TYPE1(tmp_type_arg_2);
                assert(!(tmp_source_name_1 == NULL));
                tmp_tuple_element_1 = LOOKUP_ATTRIBUTE(tmp_source_name_1, const_str_plain___name__);
                Py_DECREF(tmp_source_name_1);
                if (tmp_tuple_element_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_1);
                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                PyTuple_SET_ITEM(tmp_right_name_1, 1, tmp_tuple_element_1);
                tmp_make_exception_arg_1 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_1, tmp_right_name_1);
                Py_DECREF(tmp_right_name_1);
                if (tmp_make_exception_arg_1 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_1);
                    Py_XDECREF(exception_keeper_value_1);
                    Py_XDECREF(exception_keeper_tb_1);



                    goto try_except_handler_2;
                }
                tmp_raise_type_1 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_1);
                Py_DECREF(tmp_make_exception_arg_1);
                assert(!(tmp_raise_type_1 == NULL));
                exception_type = tmp_raise_type_1;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_2;
            }
            goto branch_end_2;
            branch_no_2:;
            // Re-raise.
            exception_type = exception_keeper_type_1;
            exception_value = exception_keeper_value_1;
            exception_tb = exception_keeper_tb_1;
            exception_lineno = exception_keeper_lineno_1;

            goto try_except_handler_2;
            branch_end_2:;
        }
        // End of try:
        try_end_1:;
        {
            PyObject *tmp_assign_source_2;
            PyObject *tmp_iter_arg_1;
            CHECK_OBJECT(tmp_dict_1__keys);
            tmp_iter_arg_1 = tmp_dict_1__keys;
            tmp_assign_source_2 = MAKE_ITERATOR(tmp_iter_arg_1);
            if (tmp_assign_source_2 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            assert(tmp_dict_1__iter == NULL);
            tmp_dict_1__iter = tmp_assign_source_2;
        }
        loop_start_1:;
        {
            PyObject *tmp_next_source_1;
            PyObject *tmp_assign_source_3;
            CHECK_OBJECT(tmp_dict_1__iter);
            tmp_next_source_1 = tmp_dict_1__iter;
            tmp_assign_source_3 = ITERATOR_NEXT(tmp_next_source_1);
            if (tmp_assign_source_3 == NULL) {
                if (CHECK_AND_CLEAR_STOP_ITERATION_OCCURRED()) {

                    goto loop_end_1;
                } else {

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                    goto try_except_handler_2;
                }
            }

            {
                PyObject *old = tmp_dict_1__key_xxx;
                tmp_dict_1__key_xxx = tmp_assign_source_3;
                Py_XDECREF(old);
            }

        }
        {
            nuitka_bool tmp_condition_result_3;
            PyObject *tmp_compexpr_left_3;
            PyObject *tmp_compexpr_right_3;
            CHECK_OBJECT(tmp_dict_1__key_xxx);
            tmp_compexpr_left_3 = tmp_dict_1__key_xxx;
            CHECK_OBJECT(par_kw);
            tmp_compexpr_right_3 = par_kw;
            tmp_res = PySequence_Contains(tmp_compexpr_right_3, tmp_compexpr_left_3);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            tmp_condition_result_3 = (tmp_res == 1) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_3 == NUITKA_BOOL_TRUE) {
                goto branch_yes_3;
            } else {
                goto branch_no_3;
            }
            branch_yes_3:;
            {
                PyObject *tmp_raise_type_2;
                PyObject *tmp_make_exception_arg_2;
                PyObject *tmp_left_name_2;
                PyObject *tmp_right_name_2;
                PyObject *tmp_tuple_element_2;
                PyObject *tmp_dircall_arg1_2;
                tmp_left_name_2 = const_str_digest_fdd1f239b4f3ffd6efcccf00e4a637e0;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_2 = par_called;
                Py_INCREF(tmp_dircall_arg1_2);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_2};
                    tmp_tuple_element_2 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                tmp_right_name_2 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_2, 0, tmp_tuple_element_2);
                CHECK_OBJECT(tmp_dict_1__key_xxx);
                tmp_tuple_element_2 = tmp_dict_1__key_xxx;
                Py_INCREF(tmp_tuple_element_2);
                PyTuple_SET_ITEM(tmp_right_name_2, 1, tmp_tuple_element_2);
                tmp_make_exception_arg_2 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_2, tmp_right_name_2);
                Py_DECREF(tmp_right_name_2);
                if (tmp_make_exception_arg_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                tmp_raise_type_2 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_2);
                Py_DECREF(tmp_make_exception_arg_2);
                assert(!(tmp_raise_type_2 == NULL));
                exception_type = tmp_raise_type_2;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_2;
            }
            branch_no_3:;
        }
        {
            PyObject *tmp_ass_subvalue_1;
            PyObject *tmp_subscribed_name_1;
            PyObject *tmp_subscript_name_1;
            PyObject *tmp_ass_subscribed_1;
            PyObject *tmp_ass_subscript_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_subscribed_name_1 = par_star_arg_dict;
            CHECK_OBJECT(tmp_dict_1__key_xxx);
            tmp_subscript_name_1 = tmp_dict_1__key_xxx;
            tmp_ass_subvalue_1 = LOOKUP_SUBSCRIPT(tmp_subscribed_name_1, tmp_subscript_name_1);
            if (tmp_ass_subvalue_1 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            CHECK_OBJECT(par_kw);
            tmp_ass_subscribed_1 = par_kw;
            CHECK_OBJECT(tmp_dict_1__key_xxx);
            tmp_ass_subscript_1 = tmp_dict_1__key_xxx;
            tmp_result = SET_SUBSCRIPT(tmp_ass_subscribed_1, tmp_ass_subscript_1, tmp_ass_subvalue_1);
            Py_DECREF(tmp_ass_subvalue_1);
            if (tmp_result == false) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
        }
        if (CONSIDER_THREADING() == false) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_2;
        }
        goto loop_start_1;
        loop_end_1:;
        goto branch_end_1;
        branch_no_1:;
        {
            nuitka_bool tmp_condition_result_4;
            int tmp_truth_name_1;
            CHECK_OBJECT(par_star_arg_dict);
            tmp_truth_name_1 = CHECK_IF_TRUE(par_star_arg_dict);
            if (tmp_truth_name_1 == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            tmp_condition_result_4 = tmp_truth_name_1 == 1 ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_4 == NUITKA_BOOL_TRUE) {
                goto branch_yes_4;
            } else {
                goto branch_no_4;
            }
            branch_yes_4:;
            {
                PyObject *tmp_assign_source_4;
                PyObject *tmp_dict_seq_1;
                CHECK_OBJECT(par_kw);
                tmp_dict_seq_1 = par_kw;
                tmp_assign_source_4 = TO_DICT(tmp_dict_seq_1, NULL);
                if (tmp_assign_source_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                {
                    PyObject *old = par_kw;
                    assert(old != NULL);
                    par_kw = tmp_assign_source_4;
                    Py_DECREF(old);
                }

            }
            {
                PyObject *tmp_assign_source_5;
                PyObject *tmp_iter_arg_2;
                PyObject *tmp_called_instance_2;
                CHECK_OBJECT(par_star_arg_dict);
                tmp_called_instance_2 = par_star_arg_dict;
                tmp_iter_arg_2 = CALL_METHOD_NO_ARGS(tmp_called_instance_2, const_str_plain_iteritems);
                if (tmp_iter_arg_2 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                tmp_assign_source_5 = MAKE_ITERATOR(tmp_iter_arg_2);
                Py_DECREF(tmp_iter_arg_2);
                if (tmp_assign_source_5 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                assert(tmp_dict_2__iter == NULL);
                tmp_dict_2__iter = tmp_assign_source_5;
            }
            loop_start_2:;
            {
                PyObject *tmp_next_source_2;
                PyObject *tmp_assign_source_6;
                CHECK_OBJECT(tmp_dict_2__iter);
                tmp_next_source_2 = tmp_dict_2__iter;
                tmp_assign_source_6 = ITERATOR_NEXT(tmp_next_source_2);
                if (tmp_assign_source_6 == NULL) {
                    if (CHECK_AND_CLEAR_STOP_ITERATION_OCCURRED()) {

                        goto loop_end_2;
                    } else {

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);


                        goto try_except_handler_2;
                    }
                }

                {
                    PyObject *old = tmp_dict_2__item;
                    tmp_dict_2__item = tmp_assign_source_6;
                    Py_XDECREF(old);
                }

            }
            {
                PyObject *tmp_assign_source_7;
                PyObject *tmp_subscribed_name_2;
                PyObject *tmp_subscript_name_2;
                CHECK_OBJECT(tmp_dict_2__item);
                tmp_subscribed_name_2 = tmp_dict_2__item;
                tmp_subscript_name_2 = const_int_0;
                tmp_assign_source_7 = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_2, tmp_subscript_name_2, 0);
                if (tmp_assign_source_7 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                {
                    PyObject *old = tmp_dict_2__key;
                    tmp_dict_2__key = tmp_assign_source_7;
                    Py_XDECREF(old);
                }

            }
            {
                nuitka_bool tmp_condition_result_5;
                PyObject *tmp_key_name_1;
                PyObject *tmp_dict_name_1;
                CHECK_OBJECT(tmp_dict_2__key);
                tmp_key_name_1 = tmp_dict_2__key;
                CHECK_OBJECT(par_kw);
                tmp_dict_name_1 = par_kw;
                tmp_res = PyDict_Contains(tmp_dict_name_1, tmp_key_name_1);
                if (tmp_res == -1) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                tmp_condition_result_5 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
                if (tmp_condition_result_5 == NUITKA_BOOL_TRUE) {
                    goto branch_yes_5;
                } else {
                    goto branch_no_5;
                }
                branch_yes_5:;
                {
                    PyObject *tmp_raise_type_3;
                    PyObject *tmp_make_exception_arg_3;
                    PyObject *tmp_left_name_3;
                    PyObject *tmp_right_name_3;
                    PyObject *tmp_tuple_element_3;
                    PyObject *tmp_dircall_arg1_3;
                    tmp_left_name_3 = const_str_digest_fdd1f239b4f3ffd6efcccf00e4a637e0;
                    CHECK_OBJECT(par_called);
                    tmp_dircall_arg1_3 = par_called;
                    Py_INCREF(tmp_dircall_arg1_3);

                    {
                        PyObject *dir_call_args[] = {tmp_dircall_arg1_3};
                        tmp_tuple_element_3 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                    }
                    if (tmp_tuple_element_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto try_except_handler_2;
                    }
                    tmp_right_name_3 = PyTuple_New(2);
                    PyTuple_SET_ITEM(tmp_right_name_3, 0, tmp_tuple_element_3);
                    CHECK_OBJECT(tmp_dict_2__key);
                    tmp_tuple_element_3 = tmp_dict_2__key;
                    Py_INCREF(tmp_tuple_element_3);
                    PyTuple_SET_ITEM(tmp_right_name_3, 1, tmp_tuple_element_3);
                    tmp_make_exception_arg_3 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_3, tmp_right_name_3);
                    Py_DECREF(tmp_right_name_3);
                    if (tmp_make_exception_arg_3 == NULL) {
                        assert(ERROR_OCCURRED());

                        FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                        goto try_except_handler_2;
                    }
                    tmp_raise_type_3 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_3);
                    Py_DECREF(tmp_make_exception_arg_3);
                    assert(!(tmp_raise_type_3 == NULL));
                    exception_type = tmp_raise_type_3;
                    RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                    goto try_except_handler_2;
                }
                branch_no_5:;
            }
            {
                PyObject *tmp_subscribed_name_3;
                PyObject *tmp_subscript_name_3;
                CHECK_OBJECT(tmp_dict_2__item);
                tmp_subscribed_name_3 = tmp_dict_2__item;
                tmp_subscript_name_3 = const_int_pos_1;
                tmp_dictset_value = LOOKUP_SUBSCRIPT_CONST(tmp_subscribed_name_3, tmp_subscript_name_3, 1);
                if (tmp_dictset_value == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
                CHECK_OBJECT(par_kw);
                tmp_dictset_dict = par_kw;
                CHECK_OBJECT(tmp_dict_2__key);
                tmp_dictset_key = tmp_dict_2__key;
                tmp_res = PyDict_SetItem(tmp_dictset_dict, tmp_dictset_key, tmp_dictset_value);
                Py_DECREF(tmp_dictset_value);
                if (tmp_res != 0) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                    goto try_except_handler_2;
                }
            }
            if (CONSIDER_THREADING() == false) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_2;
            }
            goto loop_start_2;
            loop_end_2:;
            branch_no_4:;
        }
        branch_end_1:;
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

    Py_XDECREF(tmp_dict_1__iter);
    tmp_dict_1__iter = NULL;

    Py_XDECREF(tmp_dict_1__keys);
    tmp_dict_1__keys = NULL;

    Py_XDECREF(tmp_dict_1__key_xxx);
    tmp_dict_1__key_xxx = NULL;

    Py_XDECREF(tmp_dict_2__iter);
    tmp_dict_2__iter = NULL;

    Py_XDECREF(tmp_dict_2__item);
    tmp_dict_2__item = NULL;

    Py_XDECREF(tmp_dict_2__key);
    tmp_dict_2__key = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_2;
    exception_value = exception_keeper_value_2;
    exception_tb = exception_keeper_tb_2;
    exception_lineno = exception_keeper_lineno_2;

    goto try_except_handler_1;
    // End of try:
    try_end_2:;
    Py_XDECREF(tmp_dict_1__iter);
    tmp_dict_1__iter = NULL;

    Py_XDECREF(tmp_dict_1__keys);
    tmp_dict_1__keys = NULL;

    Py_XDECREF(tmp_dict_1__key_xxx);
    tmp_dict_1__key_xxx = NULL;

    Py_XDECREF(tmp_dict_2__iter);
    tmp_dict_2__iter = NULL;

    Py_XDECREF(tmp_dict_2__item);
    tmp_dict_2__item = NULL;

    Py_XDECREF(tmp_dict_2__key);
    tmp_dict_2__key = NULL;

    {
        nuitka_bool tmp_condition_result_6;
        PyObject *tmp_compexpr_left_4;
        PyObject *tmp_compexpr_right_4;
        PyObject *tmp_type_arg_3;
        CHECK_OBJECT(par_star_arg_list);
        tmp_type_arg_3 = par_star_arg_list;
        tmp_compexpr_left_4 = BUILTIN_TYPE1(tmp_type_arg_3);
        assert(!(tmp_compexpr_left_4 == NULL));
        tmp_compexpr_right_4 = (PyObject *)&PyTuple_Type;
        tmp_condition_result_6 = (tmp_compexpr_left_4 != tmp_compexpr_right_4) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
        Py_DECREF(tmp_compexpr_left_4);
        if (tmp_condition_result_6 == NUITKA_BOOL_TRUE) {
            goto branch_yes_6;
        } else {
            goto branch_no_6;
        }
        branch_yes_6:;
        // Tried code:
        {
            PyObject *tmp_assign_source_8;
            PyObject *tmp_tuple_arg_1;
            CHECK_OBJECT(par_star_arg_list);
            tmp_tuple_arg_1 = par_star_arg_list;
            tmp_assign_source_8 = PySequence_Tuple(tmp_tuple_arg_1);
            if (tmp_assign_source_8 == NULL) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




                goto try_except_handler_4;
            }
            {
                PyObject *old = par_star_arg_list;
                assert(old != NULL);
                par_star_arg_list = tmp_assign_source_8;
                Py_DECREF(old);
            }

        }
        goto try_end_3;
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

        {
            nuitka_bool tmp_condition_result_7;
            PyObject *tmp_compexpr_left_5;
            PyObject *tmp_compexpr_right_5;
            tmp_compexpr_left_5 = exception_keeper_type_3;
            tmp_compexpr_right_5 = PyExc_TypeError;
            tmp_res = EXCEPTION_MATCH_BOOL(tmp_compexpr_left_5, tmp_compexpr_right_5);
            if (tmp_res == -1) {
                assert(ERROR_OCCURRED());

                FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                Py_DECREF(exception_keeper_type_3);
                Py_XDECREF(exception_keeper_value_3);
                Py_XDECREF(exception_keeper_tb_3);



                goto try_except_handler_1;
            }
            tmp_condition_result_7 = (tmp_res != 0) ? NUITKA_BOOL_TRUE : NUITKA_BOOL_FALSE;
            if (tmp_condition_result_7 == NUITKA_BOOL_TRUE) {
                goto branch_yes_7;
            } else {
                goto branch_no_7;
            }
            branch_yes_7:;
            {
                PyObject *tmp_raise_type_4;
                PyObject *tmp_make_exception_arg_4;
                PyObject *tmp_left_name_4;
                PyObject *tmp_right_name_4;
                PyObject *tmp_tuple_element_4;
                PyObject *tmp_dircall_arg1_4;
                PyObject *tmp_source_name_2;
                PyObject *tmp_type_arg_4;
                tmp_left_name_4 = const_str_digest_6fe42bc7767704653d0465c04f34143d;
                CHECK_OBJECT(par_called);
                tmp_dircall_arg1_4 = par_called;
                Py_INCREF(tmp_dircall_arg1_4);

                {
                    PyObject *dir_call_args[] = {tmp_dircall_arg1_4};
                    tmp_tuple_element_4 = impl___internal__$$$function_2_get_callable_name_desc(dir_call_args);
                }
                if (tmp_tuple_element_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_right_name_4 = PyTuple_New(2);
                PyTuple_SET_ITEM(tmp_right_name_4, 0, tmp_tuple_element_4);
                CHECK_OBJECT(par_star_arg_list);
                tmp_type_arg_4 = par_star_arg_list;
                tmp_source_name_2 = BUILTIN_TYPE1(tmp_type_arg_4);
                assert(!(tmp_source_name_2 == NULL));
                tmp_tuple_element_4 = LOOKUP_ATTRIBUTE(tmp_source_name_2, const_str_plain___name__);
                Py_DECREF(tmp_source_name_2);
                if (tmp_tuple_element_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);
                    Py_DECREF(tmp_right_name_4);
                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                PyTuple_SET_ITEM(tmp_right_name_4, 1, tmp_tuple_element_4);
                tmp_make_exception_arg_4 = BINARY_OPERATION_MOD_STR_TUPLE(tmp_left_name_4, tmp_right_name_4);
                Py_DECREF(tmp_right_name_4);
                if (tmp_make_exception_arg_4 == NULL) {
                    assert(ERROR_OCCURRED());

                    FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);

                    Py_DECREF(exception_keeper_type_3);
                    Py_XDECREF(exception_keeper_value_3);
                    Py_XDECREF(exception_keeper_tb_3);



                    goto try_except_handler_1;
                }
                tmp_raise_type_4 = CALL_FUNCTION_WITH_SINGLE_ARG(PyExc_TypeError, tmp_make_exception_arg_4);
                Py_DECREF(tmp_make_exception_arg_4);
                assert(!(tmp_raise_type_4 == NULL));
                exception_type = tmp_raise_type_4;
                RAISE_EXCEPTION_WITH_TYPE(&exception_type, &exception_value, &exception_tb);

                goto try_except_handler_1;
            }
            goto branch_end_7;
            branch_no_7:;
            // Re-raise.
            exception_type = exception_keeper_type_3;
            exception_value = exception_keeper_value_3;
            exception_tb = exception_keeper_tb_3;
            exception_lineno = exception_keeper_lineno_3;

            goto try_except_handler_1;
            branch_end_7:;
        }
        // End of try:
        try_end_3:;
        branch_no_6:;
    }
    {
        PyObject *tmp_called_name_1;
        PyObject *tmp_args_name_1;
        PyObject *tmp_left_name_5;
        PyObject *tmp_right_name_5;
        PyObject *tmp_kw_name_1;
        CHECK_OBJECT(par_called);
        tmp_called_name_1 = par_called;
        CHECK_OBJECT(par_args);
        tmp_left_name_5 = par_args;
        CHECK_OBJECT(par_star_arg_list);
        tmp_right_name_5 = par_star_arg_list;
        tmp_args_name_1 = BINARY_OPERATION_ADD_OBJECT_OBJECT(tmp_left_name_5, tmp_right_name_5);
        if (tmp_args_name_1 == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_1;
        }
        if (par_kw == NULL) {
            Py_DECREF(tmp_args_name_1);
            exception_type = PyExc_UnboundLocalError;
            Py_INCREF(exception_type);
            exception_value = PyString_FromFormat("local variable '%s' referenced before assignment", "kw");
            exception_tb = NULL;



            goto try_except_handler_1;
        }

        tmp_kw_name_1 = par_kw;
        tmp_return_value = CALL_FUNCTION(tmp_called_name_1, tmp_args_name_1, tmp_kw_name_1);
        Py_DECREF(tmp_args_name_1);
        if (tmp_return_value == NULL) {
            assert(ERROR_OCCURRED());

            FETCH_ERROR_OCCURRED(&exception_type, &exception_value, &exception_tb);




            goto try_except_handler_1;
        }
        goto try_return_handler_1;
    }
    // tried codes exits in all cases
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_4_complex_call_helper_pos_keywords_star_list_star_dict);
    return NULL;
    // Return handler code:
    try_return_handler_1:;
    Py_XDECREF(par_kw);
    par_kw = NULL;

    CHECK_OBJECT((PyObject *)par_star_arg_list);
    Py_DECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    goto function_return_exit;
    // Exception handler code:
    try_except_handler_1:;
    exception_keeper_type_4 = exception_type;
    exception_keeper_value_4 = exception_value;
    exception_keeper_tb_4 = exception_tb;
    exception_keeper_lineno_4 = exception_lineno;
    exception_type = NULL;
    exception_value = NULL;
    exception_tb = NULL;
    exception_lineno = 0;

    Py_XDECREF(par_kw);
    par_kw = NULL;

    Py_XDECREF(par_star_arg_list);
    par_star_arg_list = NULL;

    // Re-raise.
    exception_type = exception_keeper_type_4;
    exception_value = exception_keeper_value_4;
    exception_tb = exception_keeper_tb_4;
    exception_lineno = exception_keeper_lineno_4;

    goto function_exception_exit;
    // End of try:

    // Return statement must have exited already.
    NUITKA_CANNOT_GET_HERE(__internal__$$$function_4_complex_call_helper_pos_keywords_star_list_star_dict);
    return NULL;

function_exception_exit:
    CHECK_OBJECT(par_star_arg_dict);
    Py_DECREF(par_star_arg_dict);
    CHECK_OBJECT(par_args);
    Py_DECREF(par_args);
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);    assert(exception_type);
    RESTORE_ERROR_OCCURRED(exception_type, exception_value, exception_tb);

    return NULL;

function_return_exit:
   // Function cleanup code if any.
    CHECK_OBJECT(par_star_arg_dict);
    Py_DECREF(par_star_arg_dict);
    CHECK_OBJECT(par_args);
    Py_DECREF(par_args);
    CHECK_OBJECT(par_called);
    Py_DECREF(par_called);

   // Actual function exit with return value, making sure we did not make
   // the error status worse despite non-NULL return.
   CHECK_OBJECT(tmp_return_value);
   assert(had_error || !ERROR_OCCURRED());
   return tmp_return_value;
}



#if PYTHON_VERSION >= 300
static struct PyModuleDef mdef___internal__ =
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

function_impl_code functable___internal__[] = {

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

    function_impl_code *current = functable___internal__;
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

    if (offset > sizeof(functable___internal__) || offset < 0) {
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
        functable___internal__[offset],
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
        module___internal__,
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
PyObject *modulecode___internal__(char const *module_full_name) {
#if defined(_NUITKA_EXE) || PYTHON_VERSION >= 300
    static bool _init_done = false;

    // Modules might be imported repeatedly, which is to be ignored.
    if (_init_done) {
        return module___internal__;
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
    PRINT_STRING("__internal__: Calling setupMetaPathBasedLoader().\n");
#endif
    setupMetaPathBasedLoader();

#if PYTHON_VERSION >= 300
    patchInspectModule();
#endif

#endif

    /* The constants only used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("__internal__: Calling createModuleConstants().\n");
#endif
    createModuleConstants();

    /* The code objects used by this module are created now. */
#ifdef _NUITKA_TRACE
    PRINT_STRING("__internal__: Calling createModuleCodeObjects().\n");
#endif
    createModuleCodeObjects();

    // PRINT_STRING("in init__internal__\n");

    // Create the module object first. There are no methods initially, all are
    // added dynamically in actual code only.  Also no "__doc__" is initially
    // set at this time, as it could not contain NUL characters this way, they
    // are instead set in early module code.  No "self" for modules, we have no
    // use for it.
#if PYTHON_VERSION < 300
    module___internal__ = Py_InitModule4(
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
    mdef___internal__.m_name = module_full_name;
    module___internal__ = PyModule_Create(&mdef___internal__);
#endif

    moduledict___internal__ = MODULE_DICT(module___internal__);

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
        moduledict___internal__,
        (Nuitka_StringObject *)const_str_plain___compiled__,
        Nuitka_dunder_compiled_value
    );

    // Update "__package__" value to what it ought to be.
    {
#if 0
        UPDATE_STRING_DICT0(
            moduledict___internal__,
            (Nuitka_StringObject *)const_str_plain___package__,
            const_str_empty
        );
#elif 0
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___name__);

        UPDATE_STRING_DICT0(
            moduledict___internal__,
            (Nuitka_StringObject *)const_str_plain___package__,
            module_name
        );
#else

#if PYTHON_VERSION < 300
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___name__);
        char const *module_name_cstr = PyString_AS_STRING(module_name);

        char const *last_dot = strrchr(module_name_cstr, '.');

        if (last_dot != NULL)
        {
            UPDATE_STRING_DICT1(
                moduledict___internal__,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyString_FromStringAndSize(module_name_cstr, last_dot - module_name_cstr)
            );
        }
#else
        PyObject *module_name = GET_STRING_DICT_VALUE(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___name__);
        Py_ssize_t dot_index = PyUnicode_Find(module_name, const_str_dot, 0, PyUnicode_GetLength(module_name), -1);

        if (dot_index != -1)
        {
            UPDATE_STRING_DICT1(
                moduledict___internal__,
                (Nuitka_StringObject *)const_str_plain___package__,
                PyUnicode_Substring(module_name, 0, dot_index)
            );
        }
#endif
#endif
    }

    CHECK_OBJECT(module___internal__);

// Seems to work for Python2.7 out of the box, but for Python3, the module
// doesn't automatically enter "sys.modules", so do it manually.
#if PYTHON_VERSION >= 300
    {
        int r = PyDict_SetItemString(PyImport_GetModuleDict(), module_full_name, module___internal__);
        assert(r != -1);
    }
#endif

    // For deep importing of a module we need to have "__builtins__", so we set
    // it ourselves in the same way than CPython does. Note: This must be done
    // before the frame object is allocated, or else it may fail.

    if (GET_STRING_DICT_VALUE(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___builtins__) == NULL)
    {
        PyObject *value = (PyObject *)builtin_module;

        // Check if main module, not a dict then but the module itself.
#if !defined(_NUITKA_EXE) || !0
        value = PyModule_GetDict(value);
#endif

        UPDATE_STRING_DICT0(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___builtins__, value);
    }

#if PYTHON_VERSION >= 300
    UPDATE_STRING_DICT0(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___loader__, (PyObject *)&Nuitka_Loader_Type);
#endif

#if PYTHON_VERSION >= 340
// Set the "__spec__" value

#if 0
    // Main modules just get "None" as spec.
    UPDATE_STRING_DICT0(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___spec__, Py_None);
#else
    // Other modules get a "ModuleSpec" from the standard mechanism.
    {
        PyObject *bootstrap_module = PyImport_ImportModule("importlib._bootstrap");
        CHECK_OBJECT(bootstrap_module);
        PyObject *module_spec_class = PyObject_GetAttrString(bootstrap_module, "ModuleSpec");
        Py_DECREF(bootstrap_module);

        PyObject *args[] = {
            GET_STRING_DICT_VALUE(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___name__),
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

        UPDATE_STRING_DICT1(moduledict___internal__, (Nuitka_StringObject *)const_str_plain___spec__, spec_value);
    }
#endif
#endif

    // Temp variables if any


    // Module code.


    return module___internal__;
}
