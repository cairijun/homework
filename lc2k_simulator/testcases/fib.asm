init        lw      0   7   sp
            lw      0   1   n
            lw      0   4   fib-addr
            jalr    4   6
            halt
fib         lw      0   4   const-3
            add     7   4   7
            sw      7   6   3
            sw      7   1   2
            lw      0   4   const+1
            beq     1   4   basic-sit
            lw      0   4   const+2
            beq     1   4   basic-sit
            lw      0   4   const-1
            add     1   4   1
            lw      0   4   fib-addr
            jalr    4   6
            sw      7   3   1
            lw      7   1   2
            lw      0   4   const-2
            add     1   4   1
            lw      0   4   fib-addr
            jalr    4   6
            lw      7   4   1
            add     3   4   3
            lw      7   6   3
            lw      0   4   const+3
            add     7   4   7
            jalr    6   4
basic-sit   lw      0   3   const+1
            lw      7   6   3
            lw      0   4   const+3
            add     7   4   7
            jalr    6   4
sp          .fill   65535
fib-addr    .fill   fib
const-3     .fill   -3
const-2     .fill   -2
const-1     .fill   -1
const+1     .fill   1
const+2     .fill   2
const+3     .fill   3
n           .fill   20
