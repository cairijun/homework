init        lw      0   7   sp
            lw      0   1   n
            lw      0   5   const+2
            lw      0   3   const+1     fib(R5)
            lw      0   2   const+1     fib(R5 - 1)
            lw      0   4   fib-addr
            jalr    4   6
            halt
fib         beq     1   5   end
            lw      0   4   const-1
            add     7   4   7
            sw      7   6   1
            add     0   3   4
            add     3   2   3
            add     0   4   2
            lw      0   4   const+1
            add     5   4   5
            lw      7   6   1
            add     7   4   7
            beq     0   0   fib
end         jalr    6   4
sp          .fill   65535
fib-addr    .fill   fib
const-1     .fill   -1
const+1     .fill   1
const+2     .fill   2
n           .fill   20
