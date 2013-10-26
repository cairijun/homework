init        lw      0   1   n
            lw      0   3   const+1
            lw      0   7   sp
            lw      0   4   const-2
            add     7   4   7
loop        beq     1   2   after-loop
            lw      0   4   const+1
            add     2   4   2
            sw      7   1   1
            sw      7   2   2
            add     0   3   1
            lw      0   4   mul-addr
            jalr    4   6
            lw      7   2   2
            lw      7   1   1
            beq     0   0   loop
after-loop  halt
mul         lw      0   4   const-2
            add     7   4   7
            sw      7   6   2
            sw      7   5   1
            add     0   0   3
            lw      0   5   mul-mask
            lw      0   6   mul-checker
mul-iter    nand    2   5   4
            beq     4   6   mul-skip
            add     3   1   3
mul-skip    add     5   5   5
            beq     5   0   mul-finish
            add     1   1   1
            beq     0   0   mul-iter
mul-finish  lw      7   5   1
            lw      7   6   2
            lw      0   4   const+2
            add     7   4   7
            jalr    6   4
const-2     .fill   -2
const+1     .fill   1
const+2     .fill   2
sp          .fill   65535
mul-addr    .fill   mul
mul-mask    .fill   1
mul-checker .fill   -1
n           .fill   10
