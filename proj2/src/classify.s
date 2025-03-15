.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # check the # of command line args
    li t0, 5
    bne t0, a0, args_excep

    # Prologue
    addi sp, sp, -48
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)

    mv s0, a1 # argv
    mv s1, a2 # flag

	# =====================================
    # LOAD MATRICES
    # =====================================

    # Load pretrained m0
    li a0, 8
    jal malloc
    beq a0, x0, malloc_excep
    mv s2, a0 # m0.rows & m0.cols 
    lw a0, 4(s0)
    addi a1, s2, 0
    addi a2, s2, 4
    jal read_matrix
    mv s4, a0 # the pointer of m0 

    # Load pretrained m1
    li a0, 8
    jal malloc
    beq a0, x0, malloc_excep
    mv s3, a0 # m1.rows & m1.cols 
    lw a0, 8(s0)
    addi a1, s3, 0
    addi a2, s3, 4
    jal read_matrix
    mv s5, a0 # the pointer of m1 

    # Load input matrix
    li a0, 8
    jal malloc
    beq a0, x0, malloc_excep
    mv s7, a0 # input.rows & input.cols 
    lw a0, 12(s0)
    addi a1, s7, 0
    addi a2, s7, 4
    jal read_matrix
    mv s6, a0 # the pointer of input_matrix

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    # allocate memory for hidden layer 
    lw t0, 0(s2)
    lw t1, 4(s7)
    mul a0, t0, t1
    slli a0, a0, 2
    jal malloc
    beq a0, x0, malloc_excep
    mv s8, a0 # the address of hidden

    # hidden = m0 * input
    mv a0, s4
    lw a1, 0(s2)
    lw a2, 4(s2)
    mv a3, s6
    lw a4, 0(s7)
    lw a5, 4(s7)
    mv a6, s8
    jal matmul

    # ReLU
    mv a0, s8
    lw t0, 0(s2)
    lw t1, 4(s7)
    mul a1, t0, t1
    jal relu

    # allocate memory for result
    lw t0, 0(s3)
    lw t1, 4(s7)
    mul a0, t0, t1
    slli a0, a0, 2
    jal malloc
    beq a0, x0, malloc_excep
    mv s9, a0 # the address of result

    # linear layer
    mv a0, s5
    lw a1, 0(s3)
    lw a2, 4(s3)
    mv a3, s8
    lw a4, 0(s2)
    lw a5, 4(s7)
    mv a6, s9
    jal matmul

    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix
    lw a0, 16(s0)
    mv a1, s9
    lw a2, 0(s3)
    lw a3, 4(s7)
    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0, s9
    lw t0, 0(s3)
    lw t1, 4(s7)
    mul a1, t0, t1
    jal argmax
    mv s10, a0

    # Print classification
    bne s1, x0, no_print
    mv a1, s10
    jal print_int

    # Print newline afterwards for clarity
    li a1 '\n'
    jal print_char

no_print:
    # free the space
    mv a0, s2
    jal free
    mv a0, s3
    jal free
    mv a0, s4
    jal free
    mv a0, s5
    jal free
    mv a0, s6
    jal free
    mv a0, s7
    jal free
    mv a0, s8
    jal free
    mv a0, s9
    jal free

    # return the classification
    mv a0, s10

    # Epilogue
    lw s10, 44(sp)
    lw s9, 40(sp)
    lw s8, 36(sp)
    lw s7, 32(sp)
    lw s6, 28(sp)
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 48

    ret
    
malloc_excep:
    li a1, 88
    j exit2

args_excep:
    li a1, 89
    j exit2
