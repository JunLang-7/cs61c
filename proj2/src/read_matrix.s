.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
	addi sp, sp, -36
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)

    mv s0, a0
    mv s1, a1
    mv s2, a2

    # fopen(*a1, 'r')
    mv a1, s0
    li a2, 0
    jal fopen
    li t0, -1
    beq t0, a0, fopen_excep
    mv s3, a0 # file description

    # fread for the # of rows and cols
    mv a1, s3
    mv a2, s1
    li a3, 4
    jal fread
    li t0, 4
    bne t0, a0, fread_excep
    mv a1, s3
    mv a2, s2
    li a3, 4
    jal fread
    li t0, 4
    bne t0, a0, fread_excep
    lw t1, 0(s1)
    mv s1, t1 # the # of rows
    lw t2, 0(s2)
    mv s2, t2 # the # of cols

    # malloc for memory
    mul a0, s1, s2
    slli a0, a0, 2
    jal malloc
    beq a0, x0, malloc_excep
    mv s4, a0 # store the address of matrix

    mul s6, s1, s2 # total # of element
    li s7, 0 # i = 0
    mv s5, s4 # offset

loop:
    beq s6, s7, loop_end
    # fread the element
    mv a1, s3
    mv a2, s5
    li a3, 4
    jal fread
    li t0, 4
    bne t0, a0, fread_excep
    addi s7, s7, 1 # i++
    addi s5, s5, 4 # *mat++
    j loop

loop_end:
    # fclose
    mv a1, s3
    jal fclose
    li t0, -1
    beq t0, a0, fclose_excep
    
    # return value
    mv a0, s4

    # Epilogue
    lw s7, 32(sp)
    lw s6, 28(sp)
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 36

    ret

malloc_excep:
    li a1, 88
    j exit2

fopen_excep:
    li a1, 90
    j exit2

fread_excep:
    li a1, 91
    j exit2

fclose_excep:
    li a1, 92
    j exit2
