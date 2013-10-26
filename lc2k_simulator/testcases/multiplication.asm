init        lw      0   2   mcand
            lw      0   3   mplier
            lw      0   4   mask
            lw      0   6   checker
mul         nand    3   4   5
            beq     5   6   skip
            add     1   2   1
skip        add     4   4   4
            beq     4   0   finish
            add     2   2   2
            beq     0   0   mul
finish      halt
mask        .fill   1
checker     .fill   -1
mcand       .fill   32766
mplier      .fill   10383
