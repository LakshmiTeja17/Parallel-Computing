import numpy as np
from numpy.lib.scimath import log10
import sys

n = int(sys.argv[1])

u = np.random.randint(0, 100, (n, n), dtype=int)
a = np.zeros((n, n), dtype=int)

u = u % 100
for i in range(n):
    for j in range(n):
        for k in range(n):
            a[i, j] += u[i, k] * u[j, k]

with open(sys.argv[2], 'w') as f:
    s = str(n)
    digits = int(log10(n)) + 1
    s += ' ' * (9 - digits)
    s += "\n"

    for i in range(n):
        for j in range(n):
            s += str(a[i, j])
            digits = int(log10(a[i, j])) + 1
            s += ' ' * (9 - digits)
            if(j == (n-1)):
                s += "\n"
            else:
                s += " "
    f.write(s)
