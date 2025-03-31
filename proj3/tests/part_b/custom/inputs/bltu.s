addi t0, x0, 2
addi t1, x0, 3
bltu t0, t1, label
addi t0, t0, 1
label:
addi t0, t0, 2
bltu t0, t1, label
addi t0, t0, 1