import matplotlib.pyplot as plt
import numpy as np
from IPython import embed

x = []
y = []

with open('lab3.txt', 'r') as f:
    for line in f:
        tmp_x, _, tmp_y = line.split('-')
        x.append(int(tmp_x.split('TEST_ASSOCIATIVITY:')[-1].strip()))
        y.append(float(tmp_y.split('ms')[0].strip()))

x = np.array(x)
y = np.array(y)

plt.xlabel('Test Associativity')
plt.ylabel('Time (ms)')
plt.xticks(np.log2(x), x)

plt.plot(np.log2(x), y)
plt.savefig('lab3.png')