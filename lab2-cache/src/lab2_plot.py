import matplotlib.pyplot as plt
import numpy as np
from IPython import embed

x = []
y = []

with open('lab2.txt', 'r') as f:
    for line in f:
        tmp_x, _, tmp_y = line.split('-')
        x.append(int(tmp_x.split('B')[0].split('STRIDE:')[-1].strip()))
        y.append(float(tmp_y.split('ms')[0].strip()))

x = np.array(x)
y = np.array(y)

plt.xlabel('Test Line Size (B)')
plt.ylabel('Time (ms)')
plt.xticks(np.log2(x), x)

plt.plot(np.log2(x), y)
plt.savefig('lab2.png')