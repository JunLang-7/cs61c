addi s0, x0, 0
addi s1, x0, 0

# test 1: easy jump
jal x0, forward_jump  
addi s0, x0, 1        # skip this line

forward_jump:
addi s0, x0, 5       # s0 = 5
addi s1, x0, 5       # s1 = 5

# test 2: conditional jump
beq s0, s1, equal    
addi a0, x0, 1       # skip this line

equal:
addi a0, x0, 10      # a0 = 10
addi a1, x0, 20      # a1 = 20

# end of program
addi a2, x0, 30      # a2 = 30