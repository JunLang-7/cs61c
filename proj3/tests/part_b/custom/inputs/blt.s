addi t0, x0, 2
addi t1, x0, 3
blt t0, t1, label
addi t0, t0, 1
label:
addi t0, t0, 2
blt t0, t1, label
addi t0, t0, 1