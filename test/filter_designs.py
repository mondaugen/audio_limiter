import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

w=2*np.pi*0.05
r=0.99
b_peak=np.array([1,0,-1])/np.abs((1-np.exp(-2*w*complex('1j')))/(1-r+np.exp(-2*w*complex('1j'))*(r*r-r)))
a_peak=[1,-2*r*np.cos(w),r**2]

r0=0.99999
b_peak0=[1-r0]
a_peak0=[1,-r0]

r1=0.999
b_notch0=[r1,-r1]
a_notch0=[1,-r1]

for b,a,lab in [
    (b_peak,a_peak,'peak'),
    (b_peak0,a_peak0,'peak zero'),
    (b_notch0,a_notch0,'notch zero'),
    ]:
    w,h=signal.freqz(b,a,worN=1024)
    w=w/(2*np.pi)*44100
    plt.plot(w,20*np.log10(np.abs(h)),label=lab)
    print(lab)
    print(" b: " + str(b))
    print(" a: " + str(a))

plt.legend()
plt.show()
