.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:

    # Prologue
    li t0, 1
    bge a1, t0, no_exception
    li a1, 77
    j exit2

no_exception:
    addi sp, sp, -8
    sw s0, 0(sp)
    sw s1, 4(sp)
    li s0, 0  # max_index = 0
    lw s1, 0(a0) # max_val = a0[0]
    li t0, 0 # i = 0

loop_start:
    lw t1 0(a0)
    ble, t1, s1, loop_continue # if (max_val < a0[i])
    addi s1, t1, 0
    addi s0, t0, 0

loop_continue:
    addi t0, t0, 1
    addi a0, a0, 4
    bne a1, t0, loop_start

loop_end:
    addi a0, s0, 0 

    # Epilogue
    lw s1 4(sp)
    lw s0 0(sp)
    addi sp, sp, 8
    ret
