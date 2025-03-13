.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # Prologue
    li t0, 1
    bge a1, t0, loop_start
    li a1, 78
    j exit2

loop_start:
    addi a1, a1, -1
    lw t0 0(a0)
    bge t0, x0, loop_continue
    add t0, x0, x0

loop_continue:
    sw t0 0(a0)
    addi a0, a0, 4
    bne a1, x0, loop_start

loop_end:

    # Epilogue
    
	ret

# void relu(int* a0, int a1) {
#     do {
#         a1--;
#         if (*a0 < 0) {
#             *a0 = 0;
#         } 
#         a0 += 4
#     } while (a1 > 0)
# }
