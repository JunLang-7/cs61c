.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    li t0, 1
    blt a1, t0, excep1
    blt a2, t0, excep1
    blt a4, t0, excep2
    blt a5, t0, excep2
    bne a2, a4, excep3

    # Prologue
    addi sp, sp, -32
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    mv s0, a0
    mv s1, a1
    mv s2, a2
    mv s3, a3
    mv s4, a4
    mv s5, a5
    mv s6, a6
    li t0, 0 # i = 0

outer_loop_start:
    li t1, 0 # j = 0

inner_loop_start:
    mv a0, s0
    li t2, 4
    mul t2, t2, t1
    add a1, s3, t2
    mv a2, s2
    li a3, 1
    mv a4, s5

    # prologue
	addi sp, sp, -8
    sw t0, 0(sp)
    sw t1, 4(sp)

    jal dot

    sw a0, 0(s6)
    addi s6, s6, 4

    # epilogue
    lw t1, 4(sp)
    lw t0, 0(sp)
	addi sp, sp, 8

    addi t1, t1, 1 # j++
    beq t1, s5, inner_loop_end
    j inner_loop_start

inner_loop_end:
    addi t0, t0, 1 # i++
    beq t0, s1, outer_loop_end
    li t2, 4
    mul t2, t2, s2
    add s0, s0, t2
    j outer_loop_start

outer_loop_end:

    # Epilogue
    lw s6 28(sp)
    lw s5 24(sp)
    lw s4 20(sp)
    lw s3 16(sp)
    lw s2 12(sp)
    lw s1 8(sp)
    lw s0 4(sp)
    lw ra 0(sp)
    addi sp, sp, 32
    
    ret

excep1:
    li a1, 72
    j exit2

excep2:
    li a1, 73
    j exit2

excep3:
    li a1, 74
    j exit2
