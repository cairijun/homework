init        lw      0   1   n
            lw      0   2   r
            lw      0   7   init-sp
            lw      0   4   comb-addr
            jalr    4   6
            halt
comb        beq     2   0   basic-sit
            beq     1   2   basic-sit
            lw      0   4   const-4
            add     7   4   7
            sw      7   6   4           save the return address
            sw      7   2   3           save R2(r) (as a local variable)
            sw      7   1   2           save R1(n) (as a local variable)
            lw      0   4   const-1
            add     1   4   1
            lw      0   4   comb-addr
            jalr    4   6
            sw      7   3   1           save C(n - 1, r) to stack
            lw      0   4   const-1
            lw      7   1   2           get local variable n
            lw      7   2   3           get local variable r
            add     1   4   1
            add     2   4   2
            lw      0   4   comb-addr
            jalr    4   6
            lw      7   4   1
            add     3   4   3           calulate C(n - 1, r - 1) + C(n - 1, r) and save the result in R3
            lw      7   6   4           restore R6
            lw      0   4   const+4
            add     7   4   7
            jalr    6   4
basic-sit   lw      0   3   const+1
            jalr    6   4
init-sp     .fill   65535
const+1     .fill   1
const+4     .fill   4
const-1     .fill   -1
const-4     .fill   -4
comb-addr   .fill   comb
n           .fill   7
r           .fill   3
