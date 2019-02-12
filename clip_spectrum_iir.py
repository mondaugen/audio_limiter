import numpy as np
import matplotlib.pyplot as plt
import librosa
import limiter
import math


sndfile='/tmp/guitar.flac'
x,sr=librosa.load(sndfile)
lim=limiter.limiter(
buffer_size=4096,
ramp_up=int(sr*.01),
hold_time=int(sr*.25),
ramp_down=int(sr*.5),
threshold=0.5)

def pad_to_multiple(x,m):
    ceil_len = int(math.ceil(len(x)/m)*m)
    x_=np.zeros((ceil_len,),dtype=x.dtype)
    x_[:len(x)]=x
    return x_
    

def do_limiting(x):
    x_=pad_to_multiple(x,lim.buffer_size)
    x_lim=np.zeros_like(x_)
    for n in np.arange(0,len(x_),lim.buffer_size):
        x_lim[n:n+lim.buffer_size]=lim.tick(x_[n:n+lim.buffer_size])
    return x_lim

x=librosa.util.normalize(x)
X1=librosa.core.stft(x)
x2=2*x
x3=4*x

sigs=[
(x,'original'),
(x2,'doubled'),
(x3,'quadrupled')
]

clipped_sigs=[]
limited_sigs=[]
fig,axs=plt.subplots(len(sigs),2)
for i,(sig,name) in enumerate(sigs):
    sig_c=np.clip(sig,-1,1)
    clipped_sigs.append(sig_c)
    S_c=librosa.core.stft(sig_c)
    sig_l=do_limiting(sig)
    limited_sigs.append(sig_l)
    S_l=librosa.core.stft(sig_l)
    axs[i,0].imshow(20*np.log10(np.abs(S_c)),origin='lower',aspect='auto')
    axs[i,0].set_title(name+' clipped')
    axs[i,1].imshow(20*np.log10(np.abs(S_l)),origin='lower',aspect='auto')
    axs[i,1].set_title(name+' limited')

clipped_sigs_snd=np.concatenate(clipped_sigs)
limited_sigs_snd=np.concatenate(limited_sigs)

librosa.output.write_wav('/tmp/guitar_clipped.wav',clipped_sigs_snd,sr)
print(np.max(np.abs(clipped_sigs_snd)))
librosa.output.write_wav('/tmp/guitar_limited.wav',limited_sigs_snd,sr,norm=True)
print(np.max(np.abs(limited_sigs_snd)))

plt.show()

