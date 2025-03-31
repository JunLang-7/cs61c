addi t0, x0, 3
addi t1, x0, 2
bge t0, t1, label
addi t0, t0, 1
label:
addi t1, t1, 1

addi s0, x0, 2
addi s1, x0, 2
bge s0, s1, label2
addi t0, t0, 1
label2:
addi t1, t1, 1