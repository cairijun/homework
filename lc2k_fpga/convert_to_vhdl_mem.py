#!/usr/bin/env python2
import sys

buf = []
try:
    while True:
        x = input()
        if not (-2 ** 31 <= x < 2 ** 31):
            break
        buf.append(x)
except (EOFError, TypeError):
    pass

if len(sys.argv) < 2 or sys.argv[1] == '--regs':
    for addr, ins in enumerate(buf):
        if ins >= 0:
            print 'regs(%d) <= "%s";' % (addr,
                    bin(ins)[2:].rjust(32, '0'))
        else:
            print 'regs(%d) <= "%s";' % (addr,
                    bin((1 << 32) + ins)[2:])
elif sys.argv[1] == '--mem':
    mem = [['00000000'] * 8 for i in range(0x3F)]
    for addr, ins in enumerate(buf):
        if ins >= 0:
            hex_ins = hex(ins)[2:].upper().rjust(8, '0')
        else:
            hex_ins = hex((1 << 32) + ins)[2:].upper()
        mem[addr >> 3][addr & 0x7] = hex_ins

    for line_addr, line in enumerate(mem):
        print 'INIT_{} => X"{}{}{}{}{}{}{}{}",'.format(
                hex(line_addr)[2:].upper().rjust(2, '0'),
                *reversed(line))
