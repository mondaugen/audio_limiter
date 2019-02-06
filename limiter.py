import numpy as np
import pdb

# The value which is considered to be 0 on the exponentially decaying curve
DECAY_MIN=1e-3

class limiter:

    def __init__(self,
    # the number of samples to ramp to an attenuation peak
    ramp_up=100,
    # the number of samples to hold the attenuation peak
    hold_time=100,
    # the number of samples it takes to ramp down after an attenuation peak
    ramp_down=100,
    # the number of samples processed each time step
    buffer_size=32,
    # The threshold over which values are attenuated
    threshold=1):
        self.ramp_up = ramp_up
        self.ramp_down = ramp_down
        self.hold_time = hold_time
        self._curve_time = ramp_up + ramp_down + hold_time
        self.buffer_size = buffer_size
        self.threshold=threshold
        # the length of the lookahead buffer
        la_buffer_size = buffer_size + self.ramp_up
        # the attenuation buffer is longer because we store the ramp down in the
        # future as it will influence the next attenuation values
        atn_buffer_size = la_buffer_size + self.hold_time + self.ramp_down
        # Where the signal and is stored
        self.la_buffer=np.zeros((la_buffer_size,))
        self.atn_buffer=np.ones((atn_buffer_size,))
        # the prototype ramp is stored here so it needn't be calculated every
        # time
        self.proto_ramp_buf=np.concatenate((
            np.arange(self.ramp_up)/self.ramp_up,
            np.ones(self.hold_time),
            np.power(np.power(DECAY_MIN,1/self.ramp_down),
                np.arange(self.ramp_down))))
            #1-np.arange(self.ramp_down)/self.ramp_down))

    def tick(self,x):
        if len(x) != self.buffer_size:
            raise ValueError('input must equal buffer size')
        # shift in samples 
        self.la_buffer[:self.ramp_up] = self.la_buffer[self.buffer_size:]
        self.la_buffer[self.ramp_up:] = x
        # shift over attenuation curve 
        self.atn_buffer[:self._curve_time] = self.atn_buffer[self.buffer_size:]
        # New attenuation values are just 1 because we haven't determined their
        # attenuation amount yet
        self.atn_buffer[self._curve_time:] = 1
        # find peaks in the values L samples into the future where L is the ramp_up time
        peak_idcs=self.ramp_up+np.where(np.abs(
            self.la_buffer[self.ramp_up:self.ramp_up+self.buffer_size])>self.threshold)[0]
        for p in peak_idcs:
            atn_buf_seg=self.atn_buffer[p-self.ramp_up:p+self.hold_time+self.ramp_down]
            # Make a ramp around the peak value
            # TODO: Is this ramp correct?
            loc_ramp=1+self.proto_ramp_buf*(np.abs(self.la_buffer[p]/self.threshold)-1)
            # using [:] changes the original values
            atn_buf_seg[:]=np.where(
                loc_ramp>atn_buf_seg,
                loc_ramp,
                atn_buf_seg)
        self.la_buffer[:self.buffer_size] /= self.atn_buffer[:self.buffer_size]
        if np.max(np.abs(self.la_buffer[:self.buffer_size])) > 1:
            print(peak_idcs)
            pdb.set_trace()
        return self.la_buffer[:self.buffer_size]



