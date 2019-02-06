import numpy as np
import matplotlib.pyplot as plt

ramp_up=256
ramp_down=256
N=4096
x=np.zeros((N,))
x[:ramp_up] = np.arange(ramp_up)/ramp_up
x[ramp_up:ramp_up+ramp_down] = 1 - np.arange(ramp_down)/ramp_down
X=np.fft.fft(x)/N # N is integral of rectangular window
sr=44100
f=np.arange(N)/N*sr
fig,axs=plt.subplots(2,1)
axs[0].plot(np.arange(N),x)
axs[1].plot(f[:N//2],20*np.log10(np.abs(X[:N//2])))
plt.show()
