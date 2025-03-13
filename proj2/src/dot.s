.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:

    # Prologue
    li t0, 1
    blt a2, t0, excep1
    blt a3, t0, excep2
    blt a4, t0, excep2
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    li t0, 4
    mul t0, t0, a3 # v0's stride
    li t1, 4
    mul t1, t1, a4 # v1's stride
    li t2, 0 # i = 0
    li s2, 0 # dot = 0

loop_start:
    bge t2, a2, loop_end
    lw s0, 0(a0)
    lw s1, 0(a1)
    mul s0, s0, s1
    add s2, s2, s0 # dot += mul
    add a0, a0, t0
    add a1, a1, t1
    addi t2, t2, 1 # i += 1
    j loop_start

loop_end:
    addi a0, s2, 0

    # Epilogue
    lw s2, 8(sp)
    lw s1, 4(sp)
    lw s0, 0(sp)
    addi sp, sp, 12
    
    ret

excep1:
    li a1, 75
    j exit2

excep2:
    li a1, 76
    j exit2
