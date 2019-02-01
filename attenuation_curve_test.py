import numpy as np
import matplotlib.pyplot as plt

ramp1=np.array([
1,
1,
1,
1,
1,
1.66,
2.33,
3,
2.33,
1.66,
1
])

ramp2=np.array([
1,
1,
1,
2,
3,
4,
3,
2,
1,
1,
1
])

ramp=ramp1*ramp2

plt.plot(np.arange(len(ramp1)),20*np.log10(1/ramp1),label='ramp1')
plt.plot(np.arange(len(ramp2)),20*np.log10(1/ramp2),label='ramp2')
plt.plot(np.arange(len(ramp)),20*np.log10(1/ramp),label='combined')
plt.legend()
plt.show()
