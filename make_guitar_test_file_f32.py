import numpy as np
import librosa

sndfile='/tmp/guitar.flac'
x,sr=librosa.load(sndfile)
x=librosa.util.normalize(x)
x_=np.concatenate((x,2*x,4*x))
x_.astype('float32').tofile('/tmp/guitar.f32')
