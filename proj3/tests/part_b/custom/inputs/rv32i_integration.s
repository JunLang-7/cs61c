# RV32I integration test
# including R type, I type, S type, B type, U type, J type

addi s0, x0, 0
addi s1, x0, 10
addi s2, x0, 20

# R type instruction test
add t0, s1, s2       # t0 = 10 + 20 = 30
sub t1, s2, s1       # t1 = 20 - 10 = 10
and t2, s1, s2       # t2 = 10 & 20 = 0
or  t3, s1, s2       # t3 = 10 | 20 = 30
xor t4, s1, s2       # t4 = 10 ^ 20 = 30
addi t5, x0, 1       # t5 = 1
sll t6, t5, s1       # t6 = 1 << 10 = 1024
srl a0, t6, s0       # a0 = 1024 >> 0 = 1024

# I type instruction test
addi a1, t0, 5       # a1 = 30 + 5 = 35
andi a2, a1, 15      # a2 = 35 & 15 = 3
ori  a3, a2, 12      # a3 = 3 | 12 = 15
xori a4, a3, 10      # a4 = 15 ^ 10 = 5
slti a5, a4, 10      # a5 = (5 < 10) ? 1 : 0 = 1
slli a6, a5, 3       # a6 = 1 << 3 = 8
srli a7, a6, 1       # a7 = 8 >> 1 = 4

# S type instruction test
addi sp, x0, 1000    # set stack pointer
sw t0, 0(sp)
sw t1, 4(sp)
sh t2, 8(sp)
sb t3, 10(sp)

# U type instruction test
lui s3, 1            # s3 = 1 << 12 = 4096
auipc s4, 2          # s4 = PC + (2 << 12)

# I type instruction test (lw, lh, lb)
lw s5, 0(sp)         
lh s6, 4(sp)
lb s7, 10(sp)

# B type instruction test
beq s0, x0, beq_target  
addi s0, s0, 1          # skip this line

beq_target:
addi s0, x0, 5          # s0 = 5
bne s0, x0, bne_target  
addi s0, s0, 1          # skip this line

bne_target:
addi s0, x0, 3          # s0 = 3
addi s1, x0, 5          # s1 = 5
blt s0, s1, blt_target  
addi s0, s0, 1          # skip this line

blt_target:
bge s1, s0, bge_target  
addi s0, s0, 1          # skip this line

bge_target:
addi s0, x0, 10         # s0 = 10 (unsigned)
addi s1, x0, -1         # s1 = -1 (unsigned big integer)
bltu s0, s1, bltu_target 
addi s0, s0, 1          # skip this line

bltu_target:
bgeu s1, s0, bgeu_target 
addi s0, s0, 1          # skip this line

bgeu_target:
# J type instruction test
jal ra, jal_target      
addi s0, s0, 1          # skip this line

jal_target:

# 结束程序
addi a0, x0, 10         # 设置返回值