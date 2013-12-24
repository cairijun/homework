#!/usr/bin/env python2

buf = []
try:
    while True:
        x = input()
        if not (-2 ** 31 <= x < 2 ** 31):
            break
        buf.append(x)
except (EOFError, TypeError):
    pass

for addr, ins in enumerate(buf):
    if ins >= 0:
        print 'regs(%d) <= "%s";' % (addr,
                bin(ins)[2:].rjust(32, '0'))
    else:
        print 'regs(%d) <= "%s";' % (addr,
                bin((1 << 32) + ins)[2:])
