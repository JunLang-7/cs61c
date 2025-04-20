#include "matrix.h"

/*
 * Defines the struct that represents the object
 * Has the default PyObject_HEAD so it can be a python object
 * It also has the matrix that is being wrapped
 * is of type PyObject
 */
typedef struct {
    PyObject_HEAD
    matrix* mat;
    PyObject *shape;
} Matrix61c;

/* Function definitions */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low, double high);
int init_fill(PyObject *self, int rows, int cols, double val);
int init_1d(PyObject *self, int rows, int cols, PyObject *lst);
int init_2d(PyObject *self, PyObject *lst);
void Matrix61c_dealloc(Matrix61c *self);
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds);
PyObject *Matrix61c_to_list(Matrix61c *self);
PyObject *Matrix61c_repr(PyObject *self);
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args);
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args);
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args);
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args);
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args);
PyObject *Matrix61c_neg(Matrix61c* self);
PyObject *Matrix61c_abs(Matrix61c *self);
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional);
/* Helper function */
int check_key_type(Matrix61c* self, PyObject* key, PyObject** index0, PyObject** index1);
int boundary_check(PyObject* obj, int bound);
void get_slices(Matrix61c* self, PyObject* index0, PyObject* index1, PyObject** row_slice, PyObject** col_slice);
PyObject* matrix61c_from_slice(Matrix61c* self, PyObject* row_slice, PyObject* col_slice);
int set_matrix61c_from_slice(Matrix61c* self, PyObject* row_slice, PyObject* col_slice, PyObject* v);

