# CS61CPU

Look ma, I made a CPU! Here's what I did:

- ALU: implement the ALU logic with multiplexer
- Register file: implement the Register file with multiplexer and demultiplexer
- Single stage CPU: implement a single stage cpu with the testing instruction `addi`
- Pipeline CPU: implement a two-stage pipeline cpu with the testing instruction `addi`
- Control logic: implement the control logic for the pipeline cpu
- immediate generator: implement the immediate generator for the pipeline cpu
- Branch logic: implement the branch logic for the pipeline cpu
- CSR: implement the Control Status Register for the pipeline cpu
- handling hazard: handle the control hazard for the pipeline cpu with adding `nop` in the pipeline
- generate test: generate all the unit test, integration test and edge case test for the RV32I ISA