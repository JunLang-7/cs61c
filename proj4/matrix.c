#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/
#define OPENMP_THREAD 1000000

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows < 1 || cols < 1) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return -1;
    }

    *mat = (matrix*)malloc(sizeof(matrix));
    if (*mat == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return -1;
    }

    (*mat)->rows = rows;
    (*mat)->cols = cols;

    (*mat)->data = (double**)malloc(sizeof(double*) * rows);
    if ((*mat)->data == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        free(*(mat));
        return -1;
    }
    double *data_array = (double*)calloc(cols * rows, sizeof(double));
    if (data_array == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        free((*mat)->data);
        free(*(mat));
        return -1;
    }
    if (rows * cols > OPENMP_THREAD) {
        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < rows; i += 4) {
                if (i + 3 < rows) {
                    (*mat)->data[i] = data_array + i * cols;
                    (*mat)->data[i+1] = data_array + (i+1) * cols;
                    (*mat)->data[i+2] = data_array + (i+2) * cols;
                    (*mat)->data[i+3] = data_array + (i+3) * cols;
                } else {
                    // tail case
                    for (int j = i; j < rows; j++) {
                        (*mat)->data[j] = data_array + j * cols;
                    }
                }
            }
        }
    } 
    else {
        for (int i = 0; i < rows; i++) {
            (*mat)->data[i] = data_array + i * cols;
        }
    }

    (*mat)->is_1d = rows == 1 || cols == 1;
    (*mat)->ref_cnt = 1;
    (*mat)->parent = NULL;
    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows < 1 || cols < 1) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return -1;
    }

    *mat = (matrix*)malloc(sizeof(matrix));
    if (*mat == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Allocate matrix failed");
        return -1;
    }

    (*mat)->rows = rows;
    (*mat)->cols = cols;

    (*mat)->data = from->data + row_offset;
    if (rows * cols > OPENMP_THREAD) { 
        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < rows; i += 4) {
                if (i + 3 < rows) {
                    (*mat)->data[i] = from->data[i+row_offset] + col_offset;
                    (*mat)->data[i+1] = from->data[i+row_offset+1] + col_offset;
                    (*mat)->data[i+2] = from->data[i+row_offset+2] + col_offset;
                    (*mat)->data[i+3] = from->data[i+row_offset+3] + col_offset;
                } else {
                    // tail case
                    for (int j = i; j < rows; j++) {
                        (*mat)->data[j] = from->data[j+row_offset] + col_offset;
                    }
                }
            }
        }
    } 
    else {
        for (int i = 0; i < rows; i++) {
            (*mat)->data[i] = from->data[i+row_offset] + col_offset;
        }
    }

    (*mat)->is_1d = rows == 1 || cols == 1;
    (*mat)->ref_cnt = 1;
    from->ref_cnt++;
    (*mat)->parent = from;
    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat == NULL) {
        return;
    }
    mat->ref_cnt--;
    if (mat->ref_cnt > 0) {
        return;
    }

    if (mat->parent != NULL) {
        deallocate_matrix(mat->parent);
        free(mat);
    } else {
        free(*(mat->data));
        free(mat->data);
        free(mat);
    }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    // for (int i = 0; i < mat->rows; i++) {
    //     for (int j = 0; j < mat->cols; j++) {
    //         mat->data[i][j] = val;
    //     }
    // }
    int idx = mat->rows * mat->cols / 16 * 16;
    __m256d _val = _mm256_set1_pd(val);
    #pragma omp parallel for
    for (int i = 0; i < idx; i++) {
        _mm256_store_pd(*(mat->data) + i, _val);
        _mm256_store_pd(*(mat->data) + i + 4, _val);
        _mm256_store_pd(*(mat->data) + i + 8, _val);
        _mm256_store_pd(*(mat->data) + i + 12, _val);
    }
    // tail case
    for (int i = idx; i < mat->rows * mat->cols; i++) {
        *(*(mat->data) + i) = val;
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat1->rows || result->rows != mat2->rows 
        || result->cols != mat1->cols || result->cols != mat2->cols) {
        return -2;
    }
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        return -3;
    }
    // for (int i = 0; i < result->rows; i++) {
    //     for (int j = 0; j < result->cols; j++) {
    //         result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
    //     }
    // }
    int idx = result->rows * result->cols / 16 * 16;
    __m256d val_a, val_b, val_c, val_d, val_e, val_f, val_g, val_h;
    
    if (result->rows * result->cols > OPENMP_THREAD) {
        #pragma omp parallel for private(val_a, val_b, val_c, val_d, val_e, val_f, val_g, val_h)
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat1->data) + i);
            val_b = _mm256_loadu_pd(*(mat2->data) + i);
            val_c = _mm256_loadu_pd(*(mat1->data) + i + 4);
            val_d = _mm256_loadu_pd(*(mat2->data) + i + 4);
            val_e = _mm256_loadu_pd(*(mat1->data) + i + 8);
            val_f = _mm256_loadu_pd(*(mat2->data) + i + 8);
            val_g = _mm256_loadu_pd(*(mat1->data) + i + 12);
            val_h = _mm256_loadu_pd(*(mat2->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_add_pd(val_a, val_b));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_add_pd(val_c, val_d));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_add_pd(val_e, val_f));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_add_pd(val_g, val_h));
        }
    }
    else {
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat1->data) + i);
            val_b = _mm256_loadu_pd(*(mat2->data) + i);
            val_c = _mm256_loadu_pd(*(mat1->data) + i + 4);
            val_d = _mm256_loadu_pd(*(mat2->data) + i + 4);
            val_e = _mm256_loadu_pd(*(mat1->data) + i + 8);
            val_f = _mm256_loadu_pd(*(mat2->data) + i + 8);
            val_g = _mm256_loadu_pd(*(mat1->data) + i + 12);
            val_h = _mm256_loadu_pd(*(mat2->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_add_pd(val_a, val_b));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_add_pd(val_c, val_d));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_add_pd(val_e, val_f));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_add_pd(val_g, val_h));
        }
    }
    // tail case
    for (int i = idx; i < result->rows * result->cols; i++) {
        *(*(result->data) + i) = *(*(mat1->data) + i) + *(*(mat2->data) + i);
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    // if (result->rows != mat1->rows || result->rows != mat2->rows 
    //     || result->cols != mat1->cols || result->cols != mat2->cols) {
    //     return -2;
    // }
    // if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
    //     return -3;
    // }
    // for (int i = 0; i < result->rows; i++) {
    //     for (int j = 0; j < result->cols; j++) {
    //         result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
    //     }
    // }
    int idx = result->rows * result->cols / 16 * 16;
    __m256d val_a, val_b, val_c, val_d, val_e, val_f, val_g, val_h;
    
    if (result->rows * result->cols > OPENMP_THREAD) {
        #pragma omp parallel for private(val_a, val_b, val_c, val_d, val_e, val_f, val_g, val_h)
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat1->data) + i);
            val_b = _mm256_loadu_pd(*(mat2->data) + i);
            val_c = _mm256_loadu_pd(*(mat1->data) + i + 4);
            val_d = _mm256_loadu_pd(*(mat2->data) + i + 4);
            val_e = _mm256_loadu_pd(*(mat1->data) + i + 8);
            val_f = _mm256_loadu_pd(*(mat2->data) + i + 8);
            val_g = _mm256_loadu_pd(*(mat1->data) + i + 12);
            val_h = _mm256_loadu_pd(*(mat2->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_sub_pd(val_a, val_b));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_sub_pd(val_c, val_d));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_sub_pd(val_e, val_f));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_sub_pd(val_g, val_h));
        }
    }
    else {
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat1->data) + i);
            val_b = _mm256_loadu_pd(*(mat2->data) + i);
            val_c = _mm256_loadu_pd(*(mat1->data) + i + 4);
            val_d = _mm256_loadu_pd(*(mat2->data) + i + 4);
            val_e = _mm256_loadu_pd(*(mat1->data) + i + 8);
            val_f = _mm256_loadu_pd(*(mat2->data) + i + 8);
            val_g = _mm256_loadu_pd(*(mat1->data) + i + 12);
            val_h = _mm256_loadu_pd(*(mat2->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_sub_pd(val_a, val_b));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_sub_pd(val_c, val_d));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_sub_pd(val_e, val_f));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_sub_pd(val_g, val_h));
        }
    }
    // tail case
    for (int i = idx; i < result->rows * result->cols; i++) {
        *(*(result->data) + i) = *(*(mat1->data) + i) - *(*(mat2->data) + i);
    }
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat1->rows || result->cols != mat2->cols) {
        return -2;
    }
    if (mat1->cols != mat2->rows) {
        return -3;
    }

    matrix* temp = NULL;
    int ret = allocate_matrix(&temp, result->rows, result->cols);
    if (ret != 0) {
        return ret;
    }

    // for (int j = 0; j < result->cols; j++) {
    //     for (int k = 0; k < mat1->cols; k++) {
    //         for (int i = 0; i < result->rows; i++) {
    //             temp->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
    //         }
    //     }
    // }
    __m256d left, right0, right1, right2, right3, tmp0, tmp1, tmp2, tmp3;
    if (result->rows * result->cols > OPENMP_THREAD / 10) {
        #pragma omp parallel for private(left, right0, right1, right2, right3, tmp0, tmp1, tmp2, tmp3)
        for (int i = 0; i < result->rows; i++) {
            for (int j = 0; j < result->cols / 16 * 16; j += 16) {
                tmp0 = _mm256_loadu_pd(temp->data[i] + j);
                tmp1 = _mm256_loadu_pd(temp->data[i] + j + 4);
                tmp2 = _mm256_loadu_pd(temp->data[i] + j + 8);
                tmp3 = _mm256_loadu_pd(temp->data[i] + j + 12);
                for (int k = 0; k < mat1->cols; k++) {
                    left = _mm256_set1_pd(mat1->data[i][k]);
                    right0 = _mm256_loadu_pd(mat2->data[k] + j);
                    right1 = _mm256_loadu_pd(mat2->data[k] + j + 4);
                    right2 = _mm256_loadu_pd(mat2->data[k] + j + 8);
                    right3 = _mm256_loadu_pd(mat2->data[k] + j + 12);
                    tmp0 = _mm256_fmadd_pd(left, right0, tmp0);
                    tmp1 = _mm256_fmadd_pd(left, right1, tmp1);
                    tmp2 = _mm256_fmadd_pd(left, right2, tmp2);
                    tmp3 = _mm256_fmadd_pd(left, right3, tmp3);
                }
                _mm256_storeu_pd(temp->data[i] + j, tmp0);
                _mm256_storeu_pd(temp->data[i] + j + 4, tmp1);
                _mm256_storeu_pd(temp->data[i] + j + 8, tmp2);
                _mm256_storeu_pd(temp->data[i] + j + 12, tmp3);
            }
            // tail case
            for (int j = result->cols / 16 * 16; j < result->cols; j++) {
                double sum = 0.0;
                for (int k = 0; k < mat1->cols; k++) {
                    sum += mat1->data[i][k] * mat2->data[k][j];
                }
                temp->data[i][j] = sum;
            }
        }
    }
    else
    {
        for (int i = 0; i < result->rows; i++) {
            for (int j = 0; j < result->cols / 16 * 16; j += 16) {
                tmp0 = _mm256_loadu_pd(temp->data[i] + j);
                tmp1 = _mm256_loadu_pd(temp->data[i] + j + 4);
                tmp2 = _mm256_loadu_pd(temp->data[i] + j + 8);
                tmp3 = _mm256_loadu_pd(temp->data[i] + j + 12);
                for (int k = 0; k < mat1->cols; k++) {
                    left = _mm256_set1_pd(mat1->data[i][k]);
                    right0 = _mm256_loadu_pd(mat2->data[k] + j);
                    right1 = _mm256_loadu_pd(mat2->data[k] + j + 4);
                    right2 = _mm256_loadu_pd(mat2->data[k] + j + 8);
                    right3 = _mm256_loadu_pd(mat2->data[k] + j + 12);
                    tmp0 = _mm256_fmadd_pd(left, right0, tmp0);
                    tmp1 = _mm256_fmadd_pd(left, right1, tmp1);
                    tmp2 = _mm256_fmadd_pd(left, right2, tmp2);
                    tmp3 = _mm256_fmadd_pd(left, right3, tmp3);
                }
                _mm256_storeu_pd(temp->data[i] + j, tmp0);
                _mm256_storeu_pd(temp->data[i] + j + 4, tmp1);
                _mm256_storeu_pd(temp->data[i] + j + 8, tmp2);
                _mm256_storeu_pd(temp->data[i] + j + 12, tmp3);
            }
            // tail case
            for (int j = result->cols / 16 * 16; j < result->cols; j++) {
                double sum = 0.0;
                for (int k = 0; k < mat1->cols; k++) {
                    sum += mat1->data[i][k] * mat2->data[k][j];
                }
                temp->data[i][j] = sum;
            }
        }
    }

    result->data = temp->data;
    free(temp);
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat->rows || result->cols != mat->cols) {
        return -2;
    }
    if (mat->cols != mat->rows || pow < 0) {
        return -3;
    }
    
    if (pow == 0) {
        // set to identity matrix
        for (int i = 0; i < result->rows; i++) {
            for (int j = 0; j < result->cols; j++) {
                result->data[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
        return 0;
    }
    
    // two temp matrix for calculate
    matrix *temp = NULL;
    matrix *base = NULL;
    int ret = allocate_matrix(&temp, mat->rows, mat->cols);
    if (ret != 0) return ret;
    ret = allocate_matrix(&base, mat->rows, mat->cols);
    if (ret != 0) {
        deallocate_matrix(temp);
        return ret;
    }
    
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            base->data[i][j] = mat->data[i][j];
        }
    }
    
    // set to identity matrix
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            result->data[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    
    // quick pow
    while (pow > 0) {
        if (pow & 1) { 
            // result = result * base
            for (int i = 0; i < result->rows; i++) {
                for (int j = 0; j < result->cols; j++) {
                    temp->data[i][j] = result->data[i][j];
                }
            }
            int error_code = mul_matrix(result, temp, base);
            if (error_code != 0) {
                deallocate_matrix(temp);
                deallocate_matrix(base);
                return error_code;
            }
        }
        
        // base = base * base
        if (pow > 1) {
            for (int i = 0; i < base->rows; i++) {
                for (int j = 0; j < base->cols; j++) {
                    temp->data[i][j] = base->data[i][j];
                }
            }
            int error_code = mul_matrix(base, temp, temp);
            if (error_code != 0) {
                deallocate_matrix(temp);
                deallocate_matrix(base);
                return error_code;
            }
        }
        
        pow >>= 1;
    }
    
    deallocate_matrix(temp);
    deallocate_matrix(base);
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat->rows || result->cols != mat->cols) { 
        return -2;
    }
    // for (int i = 0; i < result->rows; i++) {
    //     for (int j = 0; j < result->cols; j++) {
    //         result->data[i][j] = -mat->data[i][j];
    //     }
    // }
    uint64_t imask = 0x8000000000000000;
    double dmask;
    memcpy(&dmask, &imask, sizeof(double));
    __m256d mask = _mm256_set1_pd(dmask);
    __m256d val_a, val_b, val_c, val_d;
    int idx = result->rows * result->cols / 16 * 16;
    if (result->rows * result->cols > OPENMP_THREAD) {
        #pragma omp parallel for private(val_a, val_b, val_c, val_d)
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat->data) + i);
            val_b = _mm256_loadu_pd(*(mat->data) + i + 4);
            val_c = _mm256_loadu_pd(*(mat->data) + i + 8);
            val_d = _mm256_loadu_pd(*(mat->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_xor_pd(val_a, mask));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_xor_pd(val_b, mask));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_xor_pd(val_c, mask));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_xor_pd(val_d, mask));
        }
    }
    else {
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat->data) + i);
            val_b = _mm256_loadu_pd(*(mat->data) + i + 4);
            val_c = _mm256_loadu_pd(*(mat->data) + i + 8);
            val_d = _mm256_loadu_pd(*(mat->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_xor_pd(val_a, mask));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_xor_pd(val_b, mask));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_xor_pd(val_c, mask));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_xor_pd(val_d, mask));
        }
    }
    // tail case
    for (int i = idx; i < result->rows * result->cols; i++) {
        *(*(result->data) + i) = -*(*(mat->data) + i);
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat->rows || result->cols != mat->cols) { 
        return -2;
    }
    // for (int i = 0; i < result->rows; i++) {
    //     for (int j = 0; j < result->cols; j++) {
    //         if (mat->data[i][j] < 0) {
    //             result->data[i][j] = -mat->data[i][j];
    //         }
    //         else {
    //             result->data[i][j] = mat->data[i][j];   
    //         }
    //     }
    // }
    uint64_t imask = ~0x8000000000000000;
    double dmask;
    memcpy(&dmask, &imask, sizeof(double));
    __m256d mask = _mm256_set1_pd(dmask);
    __m256d val_a, val_b, val_c, val_d;
    int idx = result->rows * result->cols / 16 * 16;
    if (result->rows * result->cols > OPENMP_THREAD) {
        #pragma omp parallel for private(val_a, val_b, val_c, val_d)
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat->data) + i);
            val_b = _mm256_loadu_pd(*(mat->data) + i + 4);
            val_c = _mm256_loadu_pd(*(mat->data) + i + 8);
            val_d = _mm256_loadu_pd(*(mat->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_and_pd(val_a, mask));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_and_pd(val_b, mask));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_and_pd(val_c, mask));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_and_pd(val_d, mask));
        }
    }
    else {
        for (int i = 0; i < idx; i += 16) {
            val_a = _mm256_loadu_pd(*(mat->data) + i);
            val_b = _mm256_loadu_pd(*(mat->data) + i + 4);
            val_c = _mm256_loadu_pd(*(mat->data) + i + 8);
            val_d = _mm256_loadu_pd(*(mat->data) + i + 12);
            _mm256_storeu_pd(*(result->data) + i, _mm256_and_pd(val_a, mask));
            _mm256_storeu_pd(*(result->data) + i + 4, _mm256_and_pd(val_b, mask));
            _mm256_storeu_pd(*(result->data) + i + 8, _mm256_and_pd(val_c, mask));
            _mm256_storeu_pd(*(result->data) + i + 12, _mm256_and_pd(val_d, mask));
        }
    }
    for (int i = idx; i < result->rows * result->cols; i++) {
        *(*(result->data) + i) = *(*(mat->data) + i) < 0 ? -(*(*(mat->data) + i)) : *(*(mat->data) + i);
    }
    return 0;
}

