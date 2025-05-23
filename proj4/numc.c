#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject* mat = args;
    if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*) mat;
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    
    int err_code = add_matrix(result_mat, self->mat, mat61c->mat);
    if (err_code != 0) {
        if (err_code == -3) {
            PyErr_SetString(PyExc_ValueError, "Matrixes have different dimensions");
            return NULL;
        }
        else if (err_code == -2) {
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
            return NULL;
        }
    }
    
    result->mat = result_mat;
    result->shape = self->shape;
    return (PyObject*) result;
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject* mat = args;
    if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*) mat;
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    
    int err_code = sub_matrix(result_mat, self->mat, mat61c->mat);
    if (err_code != 0) {
        if (err_code == -3) {
            PyErr_SetString(PyExc_ValueError, "Matrixes have different dimensions");
        }
        else if (err_code == -2) {
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
        }
        return NULL;
    }
    
    result->mat = result_mat;
    result->shape = self->shape;
    return (PyObject*) result;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    /* TODO: YOUR CODE HERE */
    PyObject* mat = args;
    if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*) mat;
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, mat61c->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    
    int err_code = mul_matrix(result_mat, self->mat, mat61c->mat);
    if (err_code != 0) {
        if (err_code == -3) {
            PyErr_SetString(PyExc_ValueError, "Matrixes have different dimensions");
        }
        else if (err_code == -2) {
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
        }
        return NULL;
    }
    
    result->mat = result_mat;
    result->shape = get_shape(result_mat->rows, result_mat->cols);
    return (PyObject*) result;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    /* TODO: YOUR CODE HERE */
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    
    int err_code = neg_matrix(result_mat, self->mat);
    if (err_code == -2) {
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
        return NULL;
    }
    
    result->mat = result_mat;
    result->shape = self->shape;
    return (PyObject*) result;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    
    int err_code = abs_matrix(result_mat, self->mat);
    if (err_code == -2) {
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
        return NULL;
    }
    
    result->mat = result_mat;
    result->shape = self->shape;
    return (PyObject*) result;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    /* TODO: YOUR CODE HERE */
    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Invalid value for argument pow");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    matrix* result_mat = NULL;
    int alloc_failed = allocate_matrix(&result_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return NULL;
    }
    int pow_num = (int) PyLong_AsLong(pow);
    
    int err_code = pow_matrix(result_mat, self->mat, pow_num);
    if (err_code != 0) {
        if (err_code == -3) {
            PyErr_SetString(PyExc_ValueError, "a is not a square matrix or pow is negative");
            return NULL;
        }
        else if (err_code == -2) {
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: result matrix dimension doesn't match");
            return NULL;
        }
    }
    
    result->mat = result_mat;
    result->shape = self->shape;
    return (PyObject*) result;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    .nb_add = (binaryfunc)Matrix61c_add,
    .nb_subtract = (binaryfunc)Matrix61c_sub,
    .nb_multiply = (binaryfunc)Matrix61c_multiply,
    .nb_negative = (unaryfunc)Matrix61c_neg,
    .nb_absolute = (unaryfunc)Matrix61c_abs,
    .nb_power = (ternaryfunc)Matrix61c_pow
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject* row = NULL;
    PyObject* col = NULL;
    PyObject* val = NULL;
    if (!PyArg_UnpackTuple(args, "args", 3, 3, &row, &col, &val)
        || !(row && col && val 
            && PyLong_Check(row) && PyLong_Check(col)
            && (PyLong_Check(val) || !PyFloat_Check(val)))) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    int i = (int)PyLong_AsLong(row);
    int j = (int)PyLong_AsLong(col);
    if (i >= self->mat->rows || j >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Indexing out of range");
        return NULL;
    }
    double value = (double)PyFloat_AsDouble(val);
    set(self->mat, i, j, value);
    return Py_BuildValue("");
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject* row = NULL;
    PyObject* col = NULL;
    if (!PyArg_UnpackTuple(args, "args", 2, 2, &row, &col)
        || !(row && col && PyLong_Check(row) && PyLong_Check(col))) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    int i = (int)PyLong_AsLong(row);
    int j = (int)PyLong_AsLong(col);
    if (i >= self->mat->rows || j >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Indexing out of range");
        return NULL;
    }
    double value = get(self->mat, i, j);
    return PyFloat_FromDouble(value);
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "set value at the position row, col"},
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "get value at the position row, col"},
    {NULL, NULL, 0, NULL}
};

/* INDEXING */

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    /* TODO: YOUR CODE HERE */
    PyObject* index0 = NULL;
    PyObject* index1 = NULL;
    if (check_key_type(self, key, &index0, &index1)) {
        return NULL;
    }
    PyObject* row_slice = NULL;
    PyObject* col_slice = NULL;
    get_slices(self, index0, index1, &row_slice, &col_slice);
    return matrix61c_from_slice(self, row_slice, col_slice);
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    /* TODO: YOUR CODE HERE */
    PyObject* index0 = NULL;
    PyObject* index1 = NULL;
    if (check_key_type(self, key, &index0, &index1)) {
        return -1;
    }
    PyObject* row_slice = NULL;
    PyObject* col_slice = NULL;
    get_slices(self, index0, index1, &row_slice, &col_slice);
    return set_matrix61c_from_slice(self, row_slice, col_slice, v);
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}

/* 
 * Helper function: check type of the key, if it's invalid type, throw a TypeError.
 */
int check_key_type(Matrix61c* self, PyObject* key, PyObject** index0, PyObject** index1) {
    // if it's 1d matrix or 2d matrix with only one key
    if (PyLong_Check(key) || PySlice_Check(key)) {
        if (!boundary_check(key, self->mat->rows * self->mat->cols)) {
            return -1;
        }
        *index0 = key;
        return 0;
    }
    // if it's 2d matrix
    else if (PyTuple_Check(key) && !self->mat->is_1d) {
        if (PyArg_UnpackTuple(key, "key", 1, 2, index0, index1)
            && *index0 && *index1
            && (PyLong_Check(*index0) || PySlice_Check(*index0))
            && (PyLong_Check(*index1) || PySlice_Check(*index1))) {
            if (!boundary_check(*index0, self->mat->rows)
                || !boundary_check(*index1, self->mat->cols)) {
                return -1;
            }
            return 0;
        }
    }
    // other case are all Invalid
    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
    return -1;
}

/*
 * Helper function: check the boundary of indexing, if it's out of bound, throw an IndexError.
 */
int boundary_check(PyObject* obj, int bound) {
    if (PyLong_Check(obj) && (int)PyLong_AsLong(obj) >= bound) {
        PyErr_SetString(PyExc_IndexError, "Indexing out of bound");
        return 0;
    }
    return -1;
}

/* 
 * Helper function: get the slice of the matrix. 
 */
void get_slices(Matrix61c* self, PyObject* index0, PyObject* index1, PyObject** row_slice, PyObject** col_slice) {
    // if there is only one index
    if (index1 == NULL) {
        // if it's 1d matrix
        if (self->mat->is_1d) {
            if (self->mat->rows == 1) {
                *row_slice = PySlice_New(PyLong_FromLong(0), PyLong_FromLong(1), PyLong_FromLong(1));
                *col_slice = PySlice_Check(index0)
                        ? index0
                        : PySlice_New(index0, PyNumber_Add(index0, PyLong_FromLong(1)), PyLong_FromLong(1));
            }
            else {
                *col_slice = PySlice_New(PyLong_FromLong(0), PyLong_FromLong(1), PyLong_FromLong(1));
                *row_slice = PySlice_Check(index0)
                        ? index0
                        : PySlice_New(index0, PyNumber_Add(index0, PyLong_FromLong(1)), PyLong_FromLong(1));
            }
        }
        // 2d matrix
        else {
            if (PyLong_Check(index0)) {
                int value = (int)PyLong_AsLong(index0);
                *row_slice = PySlice_New(PyLong_FromLong(value), PyLong_FromLong(value + 1), PyLong_FromLong(1));
            }
            else {
                *row_slice = index0;
            }
            *col_slice = PySlice_New(PyLong_FromLong(0), PyLong_FromLong(self->mat->cols), PyLong_FromLong(1));
        }
    }
    // if there are two index
    else {
        *row_slice = PyLong_Check(index0) 
                ? PySlice_New(index0, PyNumber_Add(index0, PyLong_FromLong(1)) ,PyLong_FromLong(1))
                : index0;
        *col_slice = PyLong_Check(index1) 
                ? PySlice_New(index1, PyNumber_Add(index1, PyLong_FromLong(1)) ,PyLong_FromLong(1))
                : index1;
    }
}

/*
* Helper function: get the Matrix61c matrix from the slice
*/
PyObject* matrix61c_from_slice(Matrix61c* self, PyObject* row_slice, PyObject* col_slice) {
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    Py_ssize_t start0 = 0, stop0 = 0, step0 = 0, slicelength0 = 0;
    Py_ssize_t start1 = 0, stop1 = 0, step1 = 0, slicelength1 = 0;

    if (PySlice_GetIndicesEx(row_slice, self->mat->rows, &start0, &stop0, &step0, &slicelength0)) {
        PyErr_SetString(PyExc_RuntimeError, "Internal error: failed to get indices from slice");
        return NULL;
    }
    if (PySlice_GetIndicesEx(col_slice, self->mat->cols, &start1, &stop1, &step1, &slicelength1)) {
        PyErr_SetString(PyExc_RuntimeError, "Internal error: failed to get indices from slice");
        return NULL;
    }
    if (step0 != 1 || slicelength0 < 1 || step1 != 1 || slicelength1 < 1) {
        PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
        return NULL;
    }
    // if its slice is a 1x1 matrix, just return a single number
    if (slicelength0 == 1 && slicelength1 == 1) {
        return PyFloat_FromDouble(get(self->mat, start0, start1));
    }
    
    int alloc_failed = allocate_matrix_ref(&(result->mat), self->mat, (int)start0, (int)start1, (int)slicelength0, (int)slicelength1);
    if (alloc_failed) {
        return NULL;
    }
    result->shape = get_shape((int)slicelength0, (int)slicelength1);
    return (PyObject*)result;
}

/*
* Helper function: set the Matrix61c matrix from the slice with value v
*/
int set_matrix61c_from_slice(Matrix61c* self, PyObject* row_slice, PyObject* col_slice, PyObject* v) {
    Py_ssize_t start0 = 0, stop0 = 0, step0 = 0, slicelength0 = 0;
    Py_ssize_t start1 = 0, stop1 = 0, step1 = 0, slicelength1 = 0;

    if (PySlice_GetIndicesEx(row_slice, self->mat->rows, &start0, &stop0, &step0, &slicelength0)) {
        PyErr_SetString(PyExc_RuntimeError, "Internal error: failed to get indices from slice");
        return -1;
    }
    if (PySlice_GetIndicesEx(col_slice, self->mat->cols, &start1, &stop1, &step1, &slicelength1)) {
        PyErr_SetString(PyExc_RuntimeError, "Internal error: failed to get indices from slice");
        return -1;
    }
    if (step0 != 1 || slicelength0 < 1 || step1 != 1 || slicelength1 < 1) {
        PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
        return -1;
    }

    // 1x1 matrix
    if (slicelength0 == 1 && slicelength1 == 1) {
        // Check the value type
        if (!(PyLong_Check(v) || PyFloat_Check(v))) {
            PyErr_SetString(PyExc_TypeError, "Resulting slice is 1 by 1, but v is not a float or int.");
            return -1;
        }
        // Set the value
        set(self->mat, (int)start0, (int)start1, PyFloat_AsDouble(v));
    }
    // 1d matrix
    else if (slicelength0 == 1 || slicelength1 == 1) {
        // Check the value type
        if (!PyList_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Resulting slice is not 1 by 1, but v is not a list.");
            return -1;
        }
        // Check the length and the element type of the list
        Py_ssize_t list_size = slicelength0 != 1 ? slicelength0 : slicelength1;
        if (PyList_Size(v) != list_size) {
            PyErr_SetString(PyExc_ValueError, "Resulting slice is 1D, but v has the wrong length.");
            return -1;
        }
        for (Py_ssize_t i = 0; i < list_size; i++) {
            if (!(PyLong_Check(PyList_GetItem(v, i)) || PyFloat_Check(PyList_GetItem(v, i)))) {
                PyErr_SetString(PyExc_ValueError, "Resulting slice is 1D, but v is not a float or int.");
                return -1;
            }
        }
        // Set the value
        if (slicelength0 == 1) {
            for (Py_ssize_t i = 0; i < slicelength1; i++) {
                set(self->mat, (int)start0, (int)(start0 + i), PyFloat_AsDouble(PyList_GetItem(v, i)));
            }
        }
        else {
            for (Py_ssize_t i = 0; i < slicelength0; i++) {
                set(self->mat, (int)(start0 + i), (int)start1, PyFloat_AsDouble(PyList_GetItem(v, i)));
            }
        }
    }
    // 2d matrix
    else {
        // Check the value type
        if (!PyList_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Resulting slice is not 1 by 1, but v is not a list.");
            return -1;
        }
        // Check the length and the element type of the list
        if (PyList_Size(v) != slicelength0) {
            PyErr_SetString(PyExc_ValueError, "Resulting slice is 2D, but v has the wrong length.");
            return -1;
        }
        for (Py_ssize_t i = 0; i < slicelength0; i++) {
            PyObject* item = PyList_GetItem(v, i);
            if (!PyList_Check(item) && PyList_Size(item) != slicelength1) {
                PyErr_SetString(PyExc_ValueError, "Resulting slice is 2D, but any element of v has the wrong length.");
                return -1;
            }
            for (Py_ssize_t j = 0; j < PyList_Size(item); j++) {
                if (!(PyLong_Check(PyList_GetItem(item, j)) || PyFloat_Check(PyList_GetItem(item, j)))) {
                    PyErr_SetString(PyExc_ValueError, "Resulting slice is 2D, but any element of an element of v is not a float or int.");
                    return -1;
                }
            }
        }
        // Set the value
        for (Py_ssize_t i = 0; i < slicelength0; i++) {
            for (Py_ssize_t j = 0; j < slicelength1; j++) {
                set(self->mat, (int)(start0 + i), (int)(start1 + j), PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(v, i), j)));
            }
        }
    }
    return 0;
}
