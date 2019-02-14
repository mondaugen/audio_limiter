import numpy as np
import matplotlib.pyplot as plt
import librosa
import limiter
import math
import exp_sum_optimize2
import limiter_ir_tf
import os
import grafted_attenuation_curve as gac

sndfile='/tmp/guitar.flac'
x,sr=librosa.load(sndfile)

def pad_to_multiple(x,m):
    ceil_len = int(math.ceil(len(x)/m)*m)
    x_=np.zeros((ceil_len,),dtype=x.dtype)
    x_[:len(x)]=x
    return x_
    
def do_limiting(x,lim):
    x_=pad_to_multiple(x,lim.buffer_size)
    x_lim=np.zeros_like(x_)
    for n in np.arange(0,len(x_),lim.buffer_size):
        x_lim[n:n+lim.buffer_size]=lim.tick(x_[n:n+lim.buffer_size])
    return x_lim

def do_limiting_atn(x,lim):
    x_=pad_to_multiple(x,lim.buffer_size)
    x_lim=np.zeros_like(x_)
    x_atn=np.zeros_like(x_)
    x_per_proced=np.zeros_like(x_)
    for n in np.arange(0,len(x_),lim.buffer_size):
        (x_lim[n:n+lim.buffer_size],
        x_atn[n:n+lim.buffer_size],
        x_per_proced[n:n+lim.buffer_size])=lim.tick(
            x_[n:n+lim.buffer_size],return_atn=True)
    # align x_ to the output of the limiter
    x_[lim.ramp_up:] = x_[:-lim.ramp_up]
    x_[:lim.ramp_up] = 0
    return (x_,x_lim,x_atn,x_per_proced)

def do_limiting_asr(x):
    """ Do limiting with an ASR shaped attenuation curve """
    lim=limiter.limiter(
    buffer_size=4096,
    ramp_up=int(sr*.01),
    hold_time=int(sr*.25),
    ramp_down=int(sr*.5),
    threshold=0.5)
    return do_limiting(x,lim)

def do_limiting_iir(x):
    """ Do limiting with an IIR generated attenuation curve """
    try:
        n_p = int(os.environ['N_P'])
    except KeyError:
        n_p=300
    try:
        n_d = int(os.environ['N_D'])
    except KeyError:
        n_d=64000
    try:
        max_w = float(os.environ['MAX_W'])
    except KeyError:
        max_w=1
    b,a,n_max=exp_sum_optimize2.find_filter_coeffs(n_p,n_d,max_w)
    print('b: ' + str(b))
    print('a: ' + str(a))
    print('n_max: ' + str(n_max))
    buffer_size=256
    fwir=limiter_ir_tf.filter_w_ir(b,a,buffer_size+n_max)
    lim=limiter_ir_tf.limiter_ir_tf(fwir,buffer_size,0.5)
    return do_limiting_atn(x,lim)

def do_limiting_iir2(x):
    """ Do limiting with an IIR generated attenuation curve but whose ramp
    function is arbitrary """
    try:
        n_p = int(os.environ['N_P'])
    except KeyError:
        n_p=300
    try:
        n_d = int(os.environ['N_D'])
    except KeyError:
        n_d=64000
    w,b,a,n_max=gac.atn_fun_design_cos_filter(n_p,n_d)
    print('b: ' + str(b))
    print('a: ' + str(a))
    print('n_max: ' + str(n_max))
    buffer_size=256
    fwir=limiter_ir_tf.filter_w_ir(b,a,buffer_size+n_max,ir=w)
    lim=limiter_ir_tf.limiter_ir_tf(fwir,buffer_size,0.5)
    return do_limiting_atn(x,lim)

if __name__ == '__main__':

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
    atn_curve=[]
    orig_sigs=[]
    n_peaks_proced=[]
    fig,axs=plt.subplots(len(sigs),2)
    for i,(sig,name) in enumerate(sigs):
        sig_c=np.clip(sig,-1,1)
        clipped_sigs.append(sig_c)
        S_c=librosa.core.stft(sig_c)
        orig_sig,sig_l,sig_a,sig_pp=do_limiting_iir2(sig)
        orig_sigs.append(orig_sig)
        limited_sigs.append(sig_l)
        atn_curve.append(sig_a)
        n_peaks_proced.append(sig_pp)
        S_l=librosa.core.stft(sig_l)
        axs[i,0].imshow(20*np.log10(np.abs(S_c)),origin='lower',aspect='auto')
        axs[i,0].set_title(name+' clipped')
        axs[i,1].imshow(20*np.log10(np.abs(S_l)),origin='lower',aspect='auto')
        axs[i,1].set_title(name+' limited')

    original_sigs_snd=np.concatenate(orig_sigs)
    clipped_sigs_snd=np.concatenate(clipped_sigs)
    limited_sigs_snd=np.concatenate(limited_sigs)
    full_atn_curve=np.concatenate(atn_curve)
    full_peaks_proced=np.concatenate(n_peaks_proced)
    fig2,axs2=plt.subplots(3,1)
    axs2[0].plot(np.arange(len(full_atn_curve)),20*np.log10(full_atn_curve))
    axs2[1].plot(np.arange(len(original_sigs_snd)),full_atn_curve*np.abs(original_sigs_snd))
    axs2[2].plot(np.arange(len(full_peaks_proced)),full_peaks_proced)

    clipped_sigs_snd_norm=librosa.util.normalize(clipped_sigs_snd)*0.9
    librosa.output.write_wav('/tmp/guitar_clipped.wav',clipped_sigs_snd_norm,sr)
    print(np.max(np.abs(clipped_sigs_snd)))
    limited_sigs_snd_norm=librosa.util.normalize(limited_sigs_snd)*0.9
    librosa.output.write_wav('/tmp/guitar_limited.wav',limited_sigs_snd_norm,sr,norm=False)
    print(np.max(np.abs(limited_sigs_snd)))

    plt.show()

