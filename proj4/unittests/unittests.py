from utils import *
from unittest import TestCase

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""


class TestAdd(TestCase):
    def test_small_add(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_add(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(200, 100, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(200, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_add(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    
    def test_huge_add(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10000, 10000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10000, 10000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_add_fail_dimension(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises((RuntimeError, ValueError)):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
    
    def test_add_fail_type(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, "not a matrix"], "add")

class TestSub(TestCase):
    def test_small_sub(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_sub(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(200, 100, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(200, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_huge_sub(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10000, 10000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10000, 10000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_sub_fail_dimension(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises((ValueError, RuntimeError)):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")

    def test_sub_fail_type(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, "not a matrix"], "sub")


class TestAbs(TestCase):
    def test_small_abs(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_abs(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 200, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    
    def test_huge_abs(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(10000, 10000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_abs_fail(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat], ["not a matrix"], "abs")

class TestNeg(TestCase):
    def test_small_neg(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_neg(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 200, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_huge_neg(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(10000, 10000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_neg_fail(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat], ["not a matrix"], "neg")


class TestMul(TestCase):
    def test_small_mul(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_mul(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(200, 150, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(150, 200, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 800, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(800, 1000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    
    def test_huge_mul(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(5000, 4000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(4000, 5000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_mul_fail_dimension(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises(ValueError):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")

    def test_mul_fail_type(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, "not a matrix"], "mul")

class TestPow(TestCase):
    def test_small_pow(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat, 3], [nc_mat, 3], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_pow(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=0)
        is_correct, speed_up = compute([dp_mat, 5], [nc_mat, 5], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_pow(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=0)
        is_correct, speed_up = compute([dp_mat, 7], [nc_mat, 7], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    
    def test_huge_pow(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(5000, 5000, seed=0)
        is_correct, speed_up = compute([dp_mat, 9], [nc_mat, 9], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_pow_fail_pow_neg(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(ValueError):
            is_correct, speed_up = compute([dp_mat, 2], [nc_mat, -3], "pow")
    
    def test_pow_fail_dimension(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(ValueError):
            is_correct, speed_up = compute([dp_mat, 2], [nc_mat, 2], "pow")
        
    def test_pow_fail_type(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(TypeError):
            is_correct, speed_up = compute([dp_mat, 2], ["not a matrix", 2], "pow")


class TestGet(TestCase):
    def test_small_get(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )

    def test_medium_get(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(25, 20, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )

    def test_large_get(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(200, 250, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )
    
    def test_get_fail_type(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(TypeError):
            _ = nc_mat["not an int"][100]
    
    def test_get_fail_index(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(IndexError):
            _ = nc_mat[0][100]
    
    def test_get_fail_1d(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 1, seed=0)
        with self.assertRaises(IndexError):
            _ = nc_mat[20]

    def test_get_fail_slice(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(ValueError):
            _ = nc_mat[0:10:2]
        with self.assertRaises(ValueError):
            _ = nc_mat[0:0]

class TestSet(TestCase):
    def test_small_set(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        rand_val = np.random.uniform(-100, 100)
        dp_mat[rand_row][rand_col] = rand_val
        nc_mat[rand_row][rand_col] = rand_val
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )

    def test_medium_set(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(25, 20, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        rand_val = np.random.uniform(-100, 100)
        dp_mat[rand_row][rand_col] = rand_val
        nc_mat[rand_row][rand_col] = rand_val
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )
    
    def test_large_set(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(200, 250, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        rand_val = np.random.uniform(-100, 100)
        dp_mat[rand_row][rand_col] = rand_val
        nc_mat[rand_row][rand_col] = rand_val
        self.assertEqual(
            round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places),
        )

    def test_set_fail_index(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(IndexError):
            dp_mat[0][100] = 5
            nc_mat[0][100] = 5
    
    def test_set_fail_1d(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 1, seed=0)
        with self.assertRaises(IndexError):
            nc_mat[20] = 1

    def test_get_fail_slice(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(ValueError):
            nc_mat[0:10:2] = [1, 2, 3, 4, 5]
        with self.assertRaises(ValueError):
            nc_mat[0:0] = 0

    def test_set_fail_type(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 50, seed=0)
        with self.assertRaises(TypeError):
            dp_mat["not an int"][100] = 5
            nc_mat["not an int"][100] = 5

class TestShape(TestCase):
    def test_square_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)

    def test_1d_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 1, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)

    def test_1d_shape_2(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 1, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1, 20, seed=1)
        self.assertTrue(dp_mat1.shape == dp_mat2.shape)
    
    def test_rect_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(20, 3, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)

    def test_large_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(200, 250, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)

    def test_shape_fail(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 50, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(30, 50, seed=1)
        self.assertFalse(dp_mat1.shape == dp_mat2.shape)
