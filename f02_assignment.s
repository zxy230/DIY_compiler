.text

.globl main


main:
    # Prologue
    addi sp, sp, -32
    sw ra, 0(sp)
    # Save registers
    li t0, 0
    sw t0, 4(sp)
    li t0, 127
    sw t0, 4(sp)
    call randint
    sw a0, 4(sp)
    call EVAL
    sw a0, 4(sp)
    lw t0, 4(sp)
    sw t0, 4(sp)
    lw t0, 4(sp)
    sw t0, 4(sp)
    li t0, 0
    sw t0, 4(sp)
    li t0, 127
    sw t0, 4(sp)
    call randint
    sw a0, 4(sp)
    call EVAL
    sw a0, 4(sp)
    lw t0, 4(sp)
    lw t1, 4(sp)
    add t2, t0, t1
    sw t2, 4(sp)
    lw t0, 4(sp)
    sw t0, 4(sp)
    lw t0, 4(sp)
    sw t0, 4(sp)
    lw t0, 4(sp)
    sw t0, 0(sp)
    # Return
    # Epilogue
    lw ra, 0(sp)
    addi sp, sp, 32
    ret
