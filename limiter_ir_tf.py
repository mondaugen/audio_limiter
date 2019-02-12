import numpy as np
import scipy
from scipy import signal

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
    ):
        self.b = b
        self.a = a
        self.m = m
        ir = np.zeros((m,))
        ir[0] = 1
        # The order of the filter
        self.order=int(np.max([len(self.b),len(self.a)])-1)
        self.ir,_ = signal.lfilter(self.b,self.a,ir,zi=np.zeros((self.order,)))
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
        threshold=1):
        self.fwir=fwir
        self.buffer_size=buffer_size
        self.threshold=threshold
        self.ramp_up = self.fwir.ir_argmax
        # TODO This isn't a correct condition, what really should happen is the
        # amount of lookahead should be enough to accomodate ramp_up samples
        assert(self.ramp_up >= self.buffer_size)
        assert(self.fwir.order < self.ramp_up)
        # the length of the lookahead and attenuation buffer
        la_buffer_size = buffer_size + self.ramp_up
        # Where the signal and its attenuation is stored
        self.la_buffer=np.zeros((la_buffer_size,))
        self.atn_buffer=np.zeros((la_buffer_size,))
        #self.last_x=np.zeros((self.fwir.order,))

    def tick(self,x):
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
        peak_idcs=self.ramp_up+np.where(np.abs(
            self.la_buffer[self.ramp_up:self.ramp_up+self.buffer_size])>self.threshold)[0]
        # sort peak indices in descending order by their height
        peak_idcs=peak_idcs[np.argsort(np.abs(self.la_buffer[peak_idcs]))[::-1]]
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
            # we need to attenuate a bit more
            #atn_amt = atn_amt - (self.atn_buffer[p]+1)
            atn_amt = atn_amt - self.atn_buffer[p]
            atn_buf_seg=self.atn_buffer[p-self.ramp_up:]
            ## using [:] changes the original values
            atn_buf_seg[:]+=self.fwir.ir[:len(atn_buf_seg)]*atn_amt
        self.la_buffer[:self.buffer_size] *= 1 - self.atn_buffer[:self.buffer_size]
        if np.max(np.abs(self.la_buffer[:self.buffer_size])) > 1:
            print(peak_idcs)
            pdb.set_trace()
        return self.la_buffer[:self.buffer_size]

