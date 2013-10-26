lw 0 5 pos1 $5 = 1
lw 0 1 input $1 = memory[input]
lw 0 2 subAdr prepare to call sub4n. $2=sub4n
jalr 2 4 call sub4n; $4=return address; $3=answer
halt
sub4n sw 7 4 stack save return address on stack
add 7 5 7 increment stack pointer
sw 7 1 stack save $1 on stack
add 7 5 7 increment stack pointer
add 1 1 1 compute 2*input
add 1 1 3 compute 4*input
lw 0 2 neg1 $2 = -1
add 7 2 7 decrement stack pointer
lw 7 1 stack recover original $1
add 7 2 7 decrement stack pointer
lw 7 4 stack recover original return address
jalr 4 2 return. $2 is not restored.
pos1 .fill 1
neg1 .fill -1
subAdr .fill sub4n contains the address of sub4n
input .fill 10 input = 10
stack .fill 0 beginning of stack (value is irrelevant)
