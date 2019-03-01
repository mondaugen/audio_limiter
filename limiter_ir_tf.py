import numpy as np
import scipy
from scipy import signal
import pdb

# A limiter whose attenuation function is computed as the impulse response of a
# low-order filter. This way we don't need to store values of the cumulated
# attenuation function far into the future, saving time and space. In a
# memory-constrained application, we can store the impulse response in read-only
# memory (e.g., flash memory).

class filter_w_ir:
    # A filter whose IR is stored with it
    def __init__(self,
    b, # b coefficients
    a, # a coefficients
    m, # number of samples of IR to compute and store
    ir=None, # in the case you have an IR you already have computed, it can be passed in here
    ):
        self.b = b
        self.a = a
        self.m = m
        self.order=int(np.max([len(self.b),len(self.a)])-1)
        if ir is None:
            ir = np.zeros((m,))
            ir[0] = 1
            # The order of the filter
            self.ir,_ = signal.lfilter(self.b,self.a,ir,zi=np.zeros((self.order,)))
        else:
            self.ir=ir
        # The maximum value of the IR
        self.ir_max=np.max(self.ir)
        # The index at which it acheives this maximum
        self.ir_argmax=np.argmax(self.ir)
        print("argmax: %d" % (self.ir_argmax,))
    def filter(self,x,x_past,y_past):
        # x_past is [x[-N],x[1-N],....x[-1]] where 0 is now
        # y_past is [y[-N],y[1-N],....y[-1]]
        z_i=signal.lfiltic(self.b,self.a,y_past[::-1],x_past[::-1])
        y,_=signal.lfilter(self.b,self.a,x,zi=z_i)
        return y
    def ir_lookup(self,n):
        if (n < 0) or (n >= len(self.ir)):
            return 0
        return self.ir[n]

def ir_for_each_peak(self,peak_idcs):
    """
    For each peak, find how much it exceeds the already existing
    attenuation function and if it exceeds, sum in an ir scaled by this
    amount. So its complexity could be up to O(n^2).
    This one is nicer.
    Warning: having a DC offset makes this work needlessly hard. Best filter it out.
    """
    n_peaks_proced = 0
    for p in peak_idcs:
        # The attenuation amount
        #atn_amt = np.abs(self.la_buffer[p])/self.threshold
        # atn_amt will always be in (0,1] because threshold < abs(la_buffer[p])
        atn_amt = 1 - self.threshold/np.abs(self.la_buffer[p])
        # we add 1 to the atn_buffer values because we add 1 to the
        # attenuation values just before they divide the signal values.
        #if atn_amt <= (self.atn_buffer[p]+1):
        #    # we're already attenuating enough so we don't need any more
        #    # attenuation
        #    continue
        if self.atn_buffer[p] > atn_amt:
            # we're already attenuating enough so we don't need any more
            # attenuation
            continue
        n_peaks_proced += 1
        # we need to attenuate a bit more
        #atn_amt = atn_amt - (self.atn_buffer[p]+1)
        atn_amt = atn_amt - self.atn_buffer[p]
        atn_buf_seg=self.atn_buffer[p-self.ramp_up:]
        ## using [:] changes the original values
        # in theory this could be > 1, so we clip to 1 (in which case the
        # attenuation will be 0)
        atn_buf_seg[:]=np.clip(atn_buf_seg[:]
            +self.fwir.ir[:len(atn_buf_seg)]*atn_amt,
            0,1)
    return (self,n_peaks_proced)

def scale_ir_for_all_peaks(self,peak_idcs):
    """
    Starting with the highest peak, propose an attenuation function, then
    for all the peaks sorted in descending order of absolute height, scale
    this first IR function by the amount needed to also cover those peaks.
    Then at the end we sum in the scaled IR.
    The only issue is now the attenuation function is maybe less smooth?
    Conclusion: This sounds like shite.
    """
    n_peaks_proced = 0
    peak_idcs=sorted(peak_idcs)
    if len(peak_idcs) > 0:
        # We place the IR around the first peak because it does not extend
        # infinitely into the past
        # If it wasn't, there would be the possibility that there could be a
        # peak before the beginning of the IR. This could never be attenuated
        # because it would be impossible to scale the IR enough (the IR would be
        # 0 at this point).
        p = peak_idcs[0]
        atn_amt = 1 - self.threshold/np.abs(self.la_buffer[p])
        ir_orig = p - self.ramp_up
        # now keep adjusting atn_amt to attenuate all the peaks
        n_peaks_proced = 1
        for p in peak_idcs[1:]:
            x_p = np.abs(self.la_buffer[p])
            #a_p_x_p = x_p * self.atn_buffer[p]
            #x_p_ir = x_p * self.fwir.ir_lookup(p-ir_orig)
            #if (x_p - a_p_x_p - atn_amt*x_p_ir) > self.threshold:
            #    c = x_p - a_p_x_p - self.threshold
            #    c /= x_p_ir
            #    # A = A + c - A -> A = c
            #    if (c <= 0) or ((x_p - a_p_x_p - (atn_amt+c)*x_p_ir) > self.threshold):
            #        pdb.set_trace()
            #    atn_amt = c
            #    n_peaks_proced += 1
            atn_amt_ = 1 - self.threshold/np.abs(self.la_buffer[p])
            ir_p_i=self.fwir.ir[p-ir_orig]
            if (self.atn_buffer[p] + atn_amt*ir_p_i) > atn_amt_:
                continue
            atn_amt = (atn_amt_ - self.atn_buffer[p])/ir_p_i
        print("atn: %f" % (atn_amt,))
        atn_buf_seg=self.atn_buffer[ir_orig:]
        ## using [:] changes the original values
        atn_buf_seg[:]=np.clip(atn_buf_seg[:]+self.fwir.ir[:len(atn_buf_seg)]*atn_amt,0,1)
    return (self,n_peaks_proced)


class limiter_ir_tf:

    """
    A limiter that uses an attenuation function generated by an IIR filter.
    """
    def __init__(self,
        # The filter_w_ir that contains the transfer function and a filter
        # function for computing its future values.
        # the ir_argmax of fwir must be greater than the buffer size.
        # the order of fwir must be less than ir_argmax
        fwir,
        # the number of samples processed each time step
        buffer_size=32,
        # The threshold over which values are attenuated
        threshold=1,
        # if sum use algorithm that sums in IR each iteration
        # if scale, scale IR until it correctly attenuates all the peaks
        peak_proc_fun='sum'):
        self.fwir=fwir
        self.buffer_size=buffer_size
        self.threshold=threshold
        self.ramp_up = self.fwir.ir_argmax
        assert(self.fwir.order < self.ramp_up)
        # the length of the lookahead and attenuation buffer
        la_buffer_size = buffer_size + self.ramp_up
        # Where the signal and its attenuation is stored
        self.la_buffer=np.zeros((la_buffer_size,))
        self.atn_buffer=np.zeros((la_buffer_size,))
        if peak_proc_fun == 'sum':
            self.peak_proc_fun = ir_for_each_peak
        elif peak_proc_fun == 'scale':
            self.peak_proc_fun = scale_ir_for_all_peaks
        #self.last_x=np.zeros((self.fwir.order,))

    def tick(self,x,return_atn=False):
        if len(x) != self.buffer_size:
            raise ValueError('input must equal buffer size')
        # shift in samples 
        self.la_buffer[:self.ramp_up] = self.la_buffer[self.buffer_size:]
        self.la_buffer[self.ramp_up:] = x
        # shift over attenuation curve 
        self.atn_buffer[:self.ramp_up] = self.atn_buffer[self.buffer_size:]
        # The initial new attenuation values are computed using the past
        # attenuation values, as we model the attenuation curve as the output of
        # an IIR filter
        self.atn_buffer[self.ramp_up:] = 0
        self.atn_buffer[self.ramp_up:]=self.fwir.filter(
            self.atn_buffer[self.ramp_up:],
            [0],
            self.atn_buffer[self.ramp_up-self.fwir.order:self.ramp_up])
        # find peaks in the values L samples into the future where L is the ramp_up time
        _find_idcs=np.arange(self.ramp_up,self.ramp_up+self.buffer_size)
        peak_idcs=self.ramp_up+np.where((1-self.atn_buffer[_find_idcs])*np.abs(
            self.la_buffer[_find_idcs])>self.threshold)[0]
        # sort peak indices in descending order by their height
        peak_idcs=peak_idcs[np.argsort(np.abs(self.la_buffer[peak_idcs]))[::-1]]
        self,n_peaks_proced=self.peak_proc_fun(self,peak_idcs)
        print("atn max: %f" % (np.max(self.atn_buffer[:self.buffer_size]),))
        self.la_buffer[:self.buffer_size] *= 1 - self.atn_buffer[:self.buffer_size]
        if np.max(np.abs(self.la_buffer[:self.buffer_size])) > 1:
            print(peak_idcs)
            pdb.set_trace()
        if return_atn:
            return (self.la_buffer[:self.buffer_size],
                    1 - self.atn_buffer[:self.buffer_size],
                    n_peaks_proced)
        return self.la_buffer[:self.buffer_size]

