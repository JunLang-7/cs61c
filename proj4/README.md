# numc

Here's what I did in project 4:

### Write naive matrix operation for `numc` module.
- `allocate_matrix` Allocate space for a matrix struct pointed to by the double pointer mat with `rows` rows and `cols` columns and fill with `0`.
- `allocate_matrix_ref` Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
- `deallocate_matrix` This function will be called automatically by Python when a `numc` matrix loses all of its reference pointers and would only free `mat->data` if no other existing matrices are also referring this data array.
- `get` Return the double value of the matrix at the given row and column.
- `set` Set the value at the given row and column to val.
- `fill_matrix` Set all entries in mat to val.
- `add_matrix` Store the result of adding mat1 and mat2 to `result`.
- `sub_matrix` Store the result of subtracting mat2 from mat1 to `result`.
- `mul_matrix` Store the result of multiplying mat1 and mat2 to `result`.
- `pow_matrix` Store the result of raising mat to the (pow)th power to `result`.
- `neg_matrix` Store the result of element-wise negating mat's entries to `result`.
- `abs_matrix` Store the result of taking the absolute value element-wise to `result`.

### Building setup script for the module installation.
referencing the official tutorial to finish the setup script with `Extension` and `setup` function.


### Writing C-Python interface for `numc` with referencing official doc.

#### number method
- `Matrix61c_add` Add the second numc.Matrix (Matrix61c) object to the first one.
- `Matrix61c_sub` Subtract the second numc.Matrix (Matrix61c) object from the first one.
- `Matrix61c_mul` NOT element-wise multiplication.
- `Matrix61c_neg` Negates the given numc.Matrix.
- `Matrix61c_abs` Take the element-wise absolute value of this numc.Matrix.
- `Matrix61c_pow` Raise numc.Matrix (Matrix61c) to the `pow`th power.
- `Matrix61c_as_number` Create a PyNumberMethods struct for overloading operators with all the number methods been define.

#### instance method
- `Matrix61c_set_value` Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val. Return None in Python (this is different from returning null).
- `Matrix61c_get_value` Given a numc.Matrix `self`, parse `args` to (int) row and (int) col. Return the value at the `row`th row and `col`th column, which is a Python float/int.
- `Matrix61c_method` Create an array of PyMethodDef structs to hold the instance methods.

#### indexing
- `Matrix61c_subscript` Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
- `Matrix61c_set_subscript` Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
- `check_key_type` Helper function: check type of the key, if it's invalid type, throw a TypeError.
- `boundary_check` Helper function: check the boundary of indexing, if it's out of bound, throw an IndexError.
- `get_slice` Helper function: get the slice of the matrix. 
- `matrix61c_from_slice` Helper function: get the Matrix61c matrix from the slice.
- `set_matrix61c_from_slice` Helper function: set the Matrix61c matrix from the slice with value v

### Writing unittest for the module.
- `TestAdd` test `+` accuracy and speedup
- `TestSub` test `-` accuracy and speedup
- `TestAbs` test `abs()` accuracy and speedup
- `TestNeg` test `-` accuracy and speedup
- `TestMul` test `*` accuracy and speedup
- `TestPow` test `**` accuracy and speedup
- `TestGet` test indexing get accuracy
- `TestSet` test indexing set accuracy
- `TestShape` test shape accuracy

### Speeding up matrix operations