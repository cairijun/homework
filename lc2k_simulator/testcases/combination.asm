init        lw      0   1   n
            lw      0   2   r
            lw      0   7   init-sp
            lw      0   4   comb-addr
            jalr    4   6
            halt
comb        lw      0   4   const-4
            add     7   4   7
            sw      7   6   4           save the return address
            sw      7   5   3           save R5(used to store C(n - 1, r))
            sw      7   2   2           save R2(r) (as a local variable)
            sw      7   1   1           save R1(n) (as a local variable)
            beq     2   0   basic-sit     C(n, 0)
            beq     1   2   basic-sit     C(n, n)
            lw      0   4   const-1
            add     1   4   1
            lw      0   4   comb-addr
            jalr    4   6
            add     0   3   5           save C(n - 1, r) to R5
            lw      0   4   const-1
            lw      7   1   1           get local variable n
            lw      7   2   2           get local variable r
            add     1   4   1
            add     2   4   2
            lw      0   4   comb-addr
            jalr    4   6
            add     3   5   3           calulate C(n - 1, r - 1) + C(n - 1, r) and save the result in R3
            lw      7   5   3           restore R5
            lw      7   6   4           restore R6
            lw      0   4   const+4
            add     7   4   7
            jalr    6   4
basic-sit   lw      0   3   const+1
            lw      7   5   3           restore R5
            lw      7   6   4           restore R6
            lw      0   4   const+4
            add     7   4   7
            jalr    6   4
init-sp     .fill   65535
const+1     .fill   1
const+4     .fill   4
const-1     .fill   -1
const-4     .fill   -4
comb-addr   .fill   comb
n           .fill   7
r           .fill   3
